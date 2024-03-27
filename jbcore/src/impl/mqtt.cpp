#include "def.hpp"
#include "mqtt.hpp"

#include <MQTTAsync.h>

namespace dsg {
#define DSG_Err RV::eErrMqtt

#define DSG_MQTTCALL(funccall)                                                                     \
  {                                                                                                \
    auto ret = MQTTASYNC_SUCCESS;                                                                  \
    if ((ret = funccall) != MQTTASYNC_SUCCESS) {                                                   \
      DSG_ERROR(#funccall << ", ret=" << MQTTAsync_strerror(ret) << "/" << ret);                   \
      return DSG_Err;                                                                              \
    }                                                                                              \
  }

#define DSG_MQTTCALL_EX(funccall)                                                                  \
  {                                                                                                \
    auto ret = MQTTASYNC_SUCCESS;                                                                  \
    if ((ret = funccall) != MQTTASYNC_SUCCESS) {                                                   \
      DSG_THROW(#funccall << ", ret=" << MQTTAsync_strerror(ret) << "/" << ret)                    \
    }                                                                                              \
  }

//////////////////////////////////////////
class MqttImpl : public interface_wrapper<Mqtt> {
public:
  MqttImpl(const Config &config);
  ~MqttImpl();

  MqttImpl(const MqttImpl &) = delete;
  MqttImpl &operator=(const MqttImpl &) = delete;
  MqttImpl(MqttImpl &&) = default;
  MqttImpl &operator=(MqttImpl &&) = default;

  auto Connect() -> Result override;
  auto Disconnect() -> Result override;
  auto ReConnect() -> Result override;
  auto IsConnected() -> bool override;
  auto Send(const Topic &topic, const std::string &textMsg) -> Result override;
  auto Send(const Topic &topic, const std::vector<uint8_t> &payload) -> Result override;
  auto Subscribe(const Topic &topic) -> Result override;
  auto Unsubscribe(const Topic &topic) -> Result override;

  auto getConfig() const -> const Config &;

private:
  auto InitClient() -> Result;
  auto DeInitClient() -> void;
  auto Send(const Topic &topic, void *payload, int payloadLen) -> Result;

private:
  Config _config;
  void *_client;
};

////////////////////////////////////////////
namespace utils {
inline auto ToQos(int v) -> Mqtt::Qos {
  return v == 0 ? Mqtt::Qos::e0 : (v == 1 ? Mqtt::Qos::e1 : Mqtt::Qos::e2);
}

inline auto FromQos(Mqtt::Qos v) -> int {
  return std::underlying_type_t<Mqtt::Qos>(v);
}
static std::string to_string(MQTTAsync_failureData *response) {
  if (!response)
    return "";

  std::stringstream ss;
  ss << "{token:" << response->token << ", code:" << response->code;
  if (response->message) {
    ss << ", msg:" << response->message;
  }
  ss << "}";
  return ss.str();
}

enum class SuccessType {
  eSub,
  eSubMany,
  ePub,
  eConnect,
};

static std::string to_string(MQTTAsync_successData *response, SuccessType type) {
  if (!response)
    return "";

  std::stringstream ss;
  ss << "MQTTAsync_successData:{"
     << "token:" << response->token;
  switch (type) {
  case SuccessType::eSub:
    ss << ", qos:" << response->alt.qos;
    break;
  case SuccessType::eSubMany:
    if (response->alt.qosList) {
      // todo
    } else {
      ss << ", qos:" << response->alt.qos;
    }
    break;
  case SuccessType::ePub:
    if (response->alt.pub.destinationName) {
      ss << ", dest:" << response->alt.pub.destinationName;
    }
    break;
  case SuccessType::eConnect:
    ss << ", ver:" << response->alt.connect.MQTTVersion
       << ", sessionPresent:" << response->alt.connect.sessionPresent;
    if (response->alt.connect.serverURI) {
      ss << ", serverURI:" << response->alt.connect.serverURI;
    }
    break;
  default:
    break;
  }

  ss << "}";
  return ss.str();
}
} // namespace utils

namespace cb {
/// @brief 连接断开回调
/// @param context
/// @param cause
static void OnConnectionLost(void *context, char *cause) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._connectLostCallback;
  if (cb) {
    cb(cause ? cause : "");
  }
}

/// @brief 接收消息回调
/// @param context
/// @param topicName
/// @param topicLen
/// @param m
/// @return
static int OnMessageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *m) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._messageArrivedCallback;
  if (cb) {
    cb({{topicName, static_cast<std::size_t>(topicLen)}, utils::ToQos(m->qos)},
       {static_cast<uint8_t *>(m->payload), static_cast<std::size_t>(m->payloadlen)});
  }
  MQTTAsync_freeMessage(&m);
  MQTTAsync_free(topicName);
  return 1;
}

/// @brief 发送完毕回调
/// @param context
/// @param token
static void OnDeliveryComplete(void *context, MQTTAsync_token token) {
  // DSG_LOG("DeliveryComplete token:" << token);
}

/// @brief 连接成功回调
/// @param context
/// @param response
static void OnConnect(void *context, MQTTAsync_successData *response) {
  // DSG_LOG("OnConnect:" << to_string(response, SuccessType::eConnect));
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._connectCallback;
  if (cb) {
    cb(true, "");
  }
}

/// @brief 连接失败回调
/// @param context
/// @param response
static void OnConnectFailure(void *context, MQTTAsync_failureData *response) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._connectCallback;
  if (cb) {
    cb(false, utils::to_string(response));
  }
}

/// @brief 断连成功回调
/// @param context
/// @param response
static void OnDisconnect(void *context, MQTTAsync_successData *response) {
  // DSG_LOG("OnDisconnect:" << to_string(response, SuccessType::eConnect));
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._disconnectCallback;
  if (cb) {
    cb(true, "");
  }
}

/// @brief 断连失败回调
/// @param context
/// @param response
static void OnDisconnectFailure(void *context, MQTTAsync_failureData *response) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._disconnectCallback;
  if (cb) {
    cb(false, utils::to_string(response));
  }
}

/// @brief 发送成功回调
/// @param context
/// @param response
static void OnSend(void *context, MQTTAsync_successData *response) {
  // DSG_LOG("OnSend:" << to_string(response, SuccessType::ePub));
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._sendCallback;
  if (cb) {
    cb(true, {response->alt.pub.destinationName, utils::ToQos(response->alt.qos)}, "");
  }
}

/// @brief 发送失败回调
/// @param context
/// @param response
static void OnSendFailure(void *context, MQTTAsync_failureData *response) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._sendCallback;
  if (cb) {
    cb(false, {}, utils::to_string(response));
  }
}

/// @brief 订阅成功回调
/// @param context
/// @param response
static void OnSubscribe(void *context, MQTTAsync_successData *response) {
  // DSG_LOG("OnSubscribe:" << to_string(response, SuccessType::eSub));
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._subscribeCallback;
  if (cb) {
    cb(true, {}, "");
  }
}

/// @brief 订阅失败回调
/// @param context
/// @param response
static void OnSubscribeFailure(void *context, MQTTAsync_failureData *response) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._subscribeCallback;
  if (cb) {
    cb(false, {}, utils::to_string(response));
  }
}

/// @brief 取消订阅成功
/// @param context
/// @param response
static void OnUnsubscribe(void *context, MQTTAsync_successData *response) {
  // DSG_LOG("OnUnSubscribe:" << to_string(response, SuccessType::eSub));
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._unsubscribeCallback;
  if (cb) {
    cb(true, {}, "");
  }
}

static void OnUnsubscribeFailure(void *context, MQTTAsync_failureData *response) {
  auto mqtt = static_cast<MqttImpl *>(context);
  auto cb = mqtt->getConfig()._unsubscribeCallback;
  if (cb) {
    cb(false, {}, utils::to_string(response));
  }
}
} // namespace cb

//////////////////////////////////////////
MqttImpl::MqttImpl(const Config &config) : _config{config}, _client{nullptr} {
  DSG_LOG(_config.to_string());
  DSG_CALL_EX(InitClient());
}

MqttImpl::~MqttImpl() {
  DeInitClient();
}

auto MqttImpl::InitClient() -> Result {
  DSG_MQTTCALL(MQTTAsync_create(&_client, _config._address.c_str(), _config._clientID.c_str(),
                                MQTTCLIENT_PERSISTENCE_NONE, NULL));
  DSG_MQTTCALL(MQTTAsync_setCallbacks(_client, this, cb::OnConnectionLost, cb::OnMessageArrived,
                                      cb::OnDeliveryComplete));
  return RV::eSuccess;
}

auto MqttImpl::DeInitClient() -> void {
  if (_client != nullptr) {
    MQTTAsync_destroy(&_client);
  }
}

auto MqttImpl::getConfig() const -> const Config & {
  return _config;
}

auto MqttImpl::Connect() -> Result {
  MQTTAsync_connectOptions connOpts = MQTTAsync_connectOptions_initializer;
  connOpts.keepAliveInterval = _config._keepAliveInterval;
  connOpts.cleansession = _config._cleansession;
  connOpts.onSuccess = cb::OnConnect;
  connOpts.onFailure = cb::OnConnectFailure;
  connOpts.context = this;

  DSG_MQTTCALL(MQTTAsync_connect(_client, &connOpts));
  return RV::eSuccess;
}

auto MqttImpl::Disconnect() -> Result {
  MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
  opts.onSuccess = cb::OnDisconnect;
  opts.onFailure = cb::OnDisconnectFailure;
  opts.context = this;
  DSG_MQTTCALL(MQTTAsync_disconnect(_client, &opts));
  return RV::eSuccess;
}

auto MqttImpl::ReConnect() -> Result {
  DSG_MQTTCALL(MQTTAsync_reconnect(_client));
  return RV::eSuccess;
}

auto MqttImpl::IsConnected() -> bool {
  return MQTTAsync_isConnected(_client);
}

auto MqttImpl::Send(const Topic &topic, void *payload, int payloadLen) -> Result {
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  opts.onSuccess = cb::OnSend;
  opts.onFailure = cb::OnSendFailure;
  opts.context = this;

  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  pubmsg.payloadlen = payloadLen;
  pubmsg.payload = payload;
  pubmsg.qos = utils::FromQos(topic._qos);
  pubmsg.retained = 0;

  DSG_MQTTCALL(MQTTAsync_sendMessage(_client, topic._name.c_str(), &pubmsg, &opts));
  return RV::eSuccess;
}
auto MqttImpl::Send(const Topic &topic, const std::vector<uint8_t> &payload) -> Result {
  return Send(topic, const_cast<uint8_t *>(payload.data()), payload.size());
}

auto MqttImpl::Send(const Topic &topic, const std::string &textMsg) -> Result {
  return Send(topic, const_cast<char *>(textMsg.data()), textMsg.length());
}

auto MqttImpl::Subscribe(const Topic &topic) -> Result {
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  opts.onSuccess = cb::OnSubscribe;
  opts.onFailure = cb::OnSubscribeFailure;
  opts.context = this;
  DSG_MQTTCALL(
      MQTTAsync_subscribe(_client, topic._name.c_str(), utils::FromQos(topic._qos), &opts));
  return RV::eSuccess;
}

auto MqttImpl::Unsubscribe(const Topic &topic) -> Result {
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  opts.onSuccess = cb::OnUnsubscribe;
  opts.onFailure = cb::OnUnsubscribeFailure;
  opts.context = this;
  DSG_MQTTCALL(MQTTAsync_unsubscribe(_client, topic._name.c_str(), &opts));
  return RV::eSuccess;
}

///////////////////////////////////////////
auto Mqtt::Config::to_string() const -> std::string {
  return DSG_STR("MqttClient{address:" << _address << ", client_id:" << _clientID << "}");
}

auto Mqtt::Topic::to_string() const -> std::string {
  return DSG_STR("topic{" << _name << ":" << utils::FromQos(_qos) << "}");
}

auto Mqtt::Request(const Config &config) -> interface_ptr<Mqtt> {
  return make_ptr<MqttImpl>(config);
}

auto Mqtt::DumpVersion() -> void {
  MQTTAsync_nameValue *info = MQTTAsync_getVersionInfo();
  while (info) {
    if (info->name && info->value) {
      DSG_LOG(">>> " << info->name << ":" << info->value);
    } else {
      break;
    }
    ++info;
  }
}
} // namespace dsg