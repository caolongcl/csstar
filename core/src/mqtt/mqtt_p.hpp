#pragma once

#include <string>
#include <sstream>

#include "mqtt.hpp"

namespace DSG
{
  class MqttP : public Mqtt
  {
  public:
    MqttP(const ClientConfig &config);
    ~MqttP();

  private:
    auto InitClient() -> Result override;
    auto DeInitClient() -> void override;
    auto Connect(const ConnectConfig&) -> Result override;
    auto Disconnect() -> Result override;
    auto ReConnect(const ConnectConfig &) -> Result override;
    auto SendMessage(const std::string &topic, Qos qos, Payload payload) -> Result override;
    auto Subscribe(const std::string &topic, Qos qos) -> Result override;

  private:
    ClientConfig _clientConfig;
    void *_client;
  };
}