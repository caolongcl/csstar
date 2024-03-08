#pragma once

#include "result.hpp"

namespace DSG
{
  struct Mqtt
  {
    struct Config
    {
      std::string _address;
      std::string _clientID;
      auto to_string() const -> std::string;
    };

    struct  ConnectConfig
    {
      
    };
    

    virtual ~Mqtt() = default;
    virtual auto InitClient() -> Result = 0;
    virtual auto DeInitClient() -> void = 0;
    virtual auto Connect(const ConnectConfig&) -> Result = 0;
    virtual auto Disconnect() -> Result = 0;

    static auto RequestMqtt(const Config &) -> Mqtt *;
  };

}