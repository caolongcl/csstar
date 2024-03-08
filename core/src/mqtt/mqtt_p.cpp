#include "def.hpp"
#include "mqtt_p.hpp"

#include <MQTTAsync.h>

namespace DSG
{
#define ADDRESS "tcp://mqtt.eclipseprojects.io:1883"
#define CLIENTID "ExampleClientPub"
#define TOPIC "MQTT Examples"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L

#define DSG_Err RV::eErrMqtt

#define DSG_MQTTCALL(funccall)                 \
  {                                            \
    auto ret = MQTTASYNC_SUCCESS;              \
    if ((ret = funccall) != MQTTASYNC_SUCCESS) \
    {                                          \
      DSG_ERROR(#funccall << ", ret=" << ret); \
      return DSG_Err;                          \
    }                                          \
  }

#define DSG_MQTTCALL_EX(funccall)                     \
  {                                                   \
    auto ret = MQTTASYNC_SUCCESS;                     \
    if ((ret = funccall) != MQTTASYNC_SUCCESS)        \
    {                                                 \
      DSG_RUNTIME_ERROR(#funccall << ", ret=" << ret) \
    }                                                 \
  }

#define DSG_MQTTCALL_V(funccall) funccall

  ///////////////////////////////////////////
  auto Mqtt::Config::to_string() const -> std::string
  {
    std::stringstream ss;
    ss << "Mqtt::Config{address:" << _address
       << ", client_id:" << _clientID
       << "}";
    return ss.str();
  }

  auto Mqtt::RequestMqtt(const Config &config) -> Mqtt *
  {
    return new MqttP(config);
  }

  ////////////////////////////////////////////
  namespace
  {
    // MQTTAsync_failureData
    static std::string to_string(MQTTAsync_failureData *response)
    {
      if (!response)
        return "";

      std::stringstream ss;
      ss << "MQTTAsync_failureData:{"
         << "token:" << response->token
         << ", code:" << response->code;
      if (response->message)
      {
        ss << ", msg:" << response->message;
      }
      ss << "}";
      return ss.str();
    }

    static std::string to_string(MQTTAsync_successData *response)
    {
      if (!response)
        return "";

      std::stringstream ss;
      ss << "MQTTAsync_successData:{"
         << "token:" << response->token
         << ", ver:" << response->alt.connect.MQTTVersion
         << ", sessionPresent:" << response->alt.connect.sessionPresent;
      if (response->alt.connect.serverURI)
      {
        ss << ", serverURI:" << response->alt.connect.serverURI;
      }

      ss << "}";
      return ss.str();
    }
  }

  static void ConnectionLost(void *context, char *cause)
  {
    DSG_LOG("ConnectionLost");
  }

  static int MessageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *m)
  {
    DSG_LOG("MessageArrived");
    return 0;
  }

  static void DeliveryComplete(void *context, MQTTAsync_token token)
  {
    DSG_LOG("DeliveryComplete");
  }

  static void OnConnect(void *context, MQTTAsync_successData *response)
  {
    DSG_LOG("OnConnect:" << to_string(response));
    auto mqtt = static_cast<MqttP *>(context);
  }

  static void OnConnectFailure(void *context, MQTTAsync_failureData *response)
  {
    DSG_LOG("OnConnectFailure:" << to_string(response));
  }

  static void OnDisconnect(void *context, MQTTAsync_successData *response)
  {
    DSG_LOG("OnDisconnect:" << to_string(response));
  }

  static void OnDisconnectFailure(void *context, MQTTAsync_failureData *response)
  {
    DSG_LOG("OnDisconnectFailure:" << to_string(response));
  }

  //////////////////////////////////////////
  MqttP::MqttP(const Config &config)
      : _config{config}, _client{nullptr}
  {
    DSG_LOG(_config.to_string());
    DSG_CALL_EX(InitClient());
  }

  MqttP::~MqttP()
  {
    DeInitClient();
  }

  auto MqttP::InitClient() -> Result
  {
    DSG_MQTTCALL(MQTTAsync_create(&_client, _config._address.c_str(), _config._clientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL));
    DSG_MQTTCALL(MQTTAsync_setCallbacks(_client, this, ConnectionLost, MessageArrived, DeliveryComplete));
    return RV::eSuccess;
  }

  auto MqttP::DeInitClient() -> void
  {
    if (_client != nullptr)
    {
      DSG_MQTTCALL_V(MQTTAsync_destroy(&_client));
    }
  }

  auto MqttP::Connect(const ConnectConfig &connectConfig) -> Result
  {
    MQTTAsync_connectOptions connOpts = MQTTAsync_connectOptions_initializer;
    connOpts.keepAliveInterval = 20;
    connOpts.cleansession = 1;
    connOpts.onSuccess = OnConnect;
    connOpts.onFailure = OnConnectFailure;
    connOpts.context = _client;

    DSG_MQTTCALL(MQTTAsync_connect(_client, &connOpts));
    return RV::eSuccess;
  }

  auto MqttP::Disconnect() -> Result
  {
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    opts.onSuccess = OnDisconnect;
    opts.onFailure = OnDisconnectFailure;
    opts.context = _client;
    DSG_MQTTCALL(MQTTAsync_disconnect(_client, &opts));
    return RV::eSuccess;
  }
}