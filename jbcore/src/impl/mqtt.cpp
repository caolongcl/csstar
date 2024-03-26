#include "def.hpp"
#include "mqtt.hpp"

#include <MQTTAsync.h>

namespace dsg {
#define DSG_Err RV::eErrMqtt

#define DSG_MQTTCALL(funccall)                                          \
  {                                                                     \
    auto ret = MQTTASYNC_SUCCESS;                                       \
    if ((ret = funccall) != MQTTASYNC_SUCCESS) {                        \
      DSG_ERROR(#funccall << ", ret=" << MQTTAsync_strerror(ret) << "/" \
                          << ret);                                      \
      return DSG_Err;                                                   \
    }                                                                   \
  }

#define DSG_MQTTCALL_EX(funccall)                                       \
  {                                                                     \
    auto ret = MQTTASYNC_SUCCESS;                                       \
    if ((ret = funccall) != MQTTASYNC_SUCCESS) {                        \
      DSG_THROW(#funccall << ", ret=" << MQTTAsync_strerror(ret) << "/" \
                          << ret)                                       \
    }                                                                   \
  }

#define DSG_MQTTCALL_V(funccall) funccall

//////////////////////////////////////////
class MqttImpl : public interface_wrapper<Mqtt> {
 public:
  MqttImpl(const ClientConfig &config);
  ~MqttImpl();

  MqttImpl(const MqttImpl &) = delete;
  MqttImpl &operator=(const MqttImpl &) = delete;
  MqttImpl(MqttImpl &&) = default;
  MqttImpl &operator=(MqttImpl &&) = default;

 private:
  auto InitClient() -> Result override;
  auto DeInitClient() -> void override;
  auto Connect(const ConnectConfig &) -> Result override;
  auto Disconnect() -> Result override;
  auto ReConnect() -> Result override;
  auto IsConnected() -> bool override;
  auto SendMessage(const std::string &topic, Qos qos, Payload payload)
      -> Result override;
  auto Subscribe(const std::string &topic, Qos qos) -> Result override;
  auto UnSubscribe(const std::string &topic) -> Result override;

 private:
  ClientConfig _clientConfig;
  void *_client;
};

////////////////////////////////////////////
namespace {
static std::string to_string(MQTTAsync_failureData *response) {
  if (!response) return "";

  std::stringstream ss;
  ss << "MQTTAsync_failureData:{"
     << "token:" << response->token << ", code:" << response->code;
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

static std::string to_string(MQTTAsync_successData *response,
                             SuccessType type) {
  if (!response) return "";

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
}  // namespace

static void ConnectionLost(void *context, char *cause) {
  std::string c = cause ? cause : "";
  DSG_LOG("ConnectionLost:" << c);
}

static int MessageArrived(void *context, char *topicName, int topicLen,
                          MQTTAsync_message *m) {
  DSG_LOG("MessageArrived: topic:" << topicName << ", msg:" << m->payloadlen
                                   << ", " << static_cast<char *>(m->payload));
  DSG_MQTTCALL_V(MQTTAsync_freeMessage(&m));
  DSG_MQTTCALL_V(MQTTAsync_free(topicName));
  return 1;
}

static void DeliveryComplete(void *context, MQTTAsync_token token) {
  DSG_LOG("DeliveryComplete token:" << token);
}

static void OnConnect(void *context, MQTTAsync_successData *response) {
  DSG_LOG("OnConnect:" << to_string(response, SuccessType::eConnect));
  auto mqtt = static_cast<MqttImpl *>(context);
}

static void OnConnectFailure(void *context, MQTTAsync_failureData *response) {
  DSG_LOG("OnConnectFailure:" << to_string(response));
}

static void OnDisconnect(void *context, MQTTAsync_successData *response) {
  DSG_LOG("OnDisconnect:" << to_string(response, SuccessType::eConnect));
}

static void OnDisconnectFailure(void *context,
                                MQTTAsync_failureData *response) {
  DSG_LOG("OnDisconnectFailure:" << to_string(response));
}

static void OnSend(void *context, MQTTAsync_successData *response) {
  DSG_LOG("OnSend:" << to_string(response, SuccessType::ePub));
}

static void OnSendFailure(void *context, MQTTAsync_failureData *response) {
  DSG_LOG("OnSendFailure:" << to_string(response));
}

static void OnSubscribe(void *context, MQTTAsync_successData *response) {
  DSG_LOG("OnSubscribe:" << to_string(response, SuccessType::eSub));
}

static void OnSubscribeFailure(void *context, MQTTAsync_failureData *response) {
  DSG_LOG("OnSubscribeFailure:" << to_string(response));
}

static void OnUnSubscribe(void *context, MQTTAsync_successData *response) {
  DSG_LOG("OnUnSubscribe:" << to_string(response, SuccessType::eSub));
}

static void OnUnSubscribeFailure(void *context,
                                 MQTTAsync_failureData *response) {
  DSG_LOG("OnUnSubscribeFailure:" << to_string(response));
}

//////////////////////////////////////////
MqttImpl::MqttImpl(const ClientConfig &config)
    : _clientConfig{config}, _client{nullptr} {
  DSG_LOG(_clientConfig.to_string());
  DSG_CALL_EX(InitClient());
}

MqttImpl::~MqttImpl() { DeInitClient(); }

auto MqttImpl::InitClient() -> Result {
  DSG_MQTTCALL(MQTTAsync_create(&_client, _clientConfig._address.c_str(),
                                _clientConfig._clientID.c_str(),
                                MQTTCLIENT_PERSISTENCE_NONE, NULL));
  DSG_MQTTCALL(MQTTAsync_setCallbacks(_client, this, ConnectionLost,
                                      MessageArrived, DeliveryComplete));
  return RV::eSuccess;
}

auto MqttImpl::DeInitClient() -> void {
  if (_client != nullptr) {
    DSG_MQTTCALL_V(MQTTAsync_destroy(&_client));
  }
}

auto MqttImpl::Connect(const ConnectConfig &connectConfig) -> Result {
  MQTTAsync_connectOptions connOpts = MQTTAsync_connectOptions_initializer;
  connOpts.keepAliveInterval = 20;
  connOpts.cleansession = 1;
  connOpts.onSuccess = OnConnect;
  connOpts.onFailure = OnConnectFailure;
  connOpts.context = _client;

  DSG_MQTTCALL(MQTTAsync_connect(_client, &connOpts));
  return RV::eSuccess;
}

auto MqttImpl::Disconnect() -> Result {
  MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
  opts.onSuccess = OnDisconnect;
  opts.onFailure = OnDisconnectFailure;
  opts.context = _client;
  DSG_MQTTCALL(MQTTAsync_disconnect(_client, &opts));
  return RV::eSuccess;
}

auto MqttImpl::ReConnect() -> Result {
  DSG_MQTTCALL(MQTTAsync_reconnect(_client));
  return RV::eSuccess;
}

auto MqttImpl::IsConnected() -> bool { return MQTTAsync_isConnected(_client); }

auto MqttImpl::SendMessage(const std::string &topic, Qos qos, Payload payload)
    -> Result {
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  opts.onSuccess = OnSend;
  opts.onFailure = OnSendFailure;
  opts.context = _client;

  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  pubmsg.payloadlen = payload.size;
  pubmsg.payload = payload.bytes;
  pubmsg.qos = std::underlying_type_t<Qos>(qos);
  pubmsg.retained = 0;

  DSG_MQTTCALL(MQTTAsync_sendMessage(_client, topic.c_str(), &pubmsg, &opts));
  return RV::eSuccess;
}

auto MqttImpl::Subscribe(const std::string &topic, Qos qos) -> Result {
  DSG_LOG("Subscribing to topic " << topic << " for client "
                                  << _clientConfig._clientID << " using QoS "
                                  << std::underlying_type_t<Qos>(qos));
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  opts.onSuccess = OnSubscribe;
  opts.onFailure = OnSubscribeFailure;
  opts.context = _client;
  DSG_MQTTCALL(MQTTAsync_subscribe(_client, topic.c_str(),
                                   std::underlying_type_t<Qos>(qos), &opts));
  return RV::eSuccess;
}

auto MqttImpl::UnSubscribe(const std::string &topic) -> Result {
  DSG_LOG("UnSubscribe to topic " << topic << " for client "
                                  << _clientConfig._clientID);
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  opts.onSuccess = OnUnSubscribe;
  opts.onFailure = OnUnSubscribeFailure;
  opts.context = _client;
  DSG_MQTTCALL(MQTTAsync_unsubscribe(_client, topic.c_str(), &opts));
  return RV::eSuccess;
}

///////////////////////////////////////////
auto Mqtt::ClientConfig::to_string() const -> std::string {
  std::stringstream ss;
  ss << "Mqtt::ClientConfig{address:" << _address << ", client_id:" << _clientID
     << "}";
  return ss.str();
}

auto Mqtt::Request(const ClientConfig &config) -> interface_ptr<Mqtt> {
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
}  // namespace dsg