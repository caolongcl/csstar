#pragma once

#include <string>
#include <sstream>

#include "mqtt.hpp"

namespace DSG
{
  class MqttP : public Mqtt
  {
  public:
    MqttP(const Config &config);
    ~MqttP();

  private:
    auto InitClient() -> Result override;
    auto DeInitClient() -> void override;
    auto Connect(const ConnectConfig&) -> Result override;
    auto Disconnect() -> Result override;


  private:
    Config _config;
    void *_client;
  };
}