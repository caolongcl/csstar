#pragma once

#include <string>
#include "result.hpp"

namespace DSG
{
  struct Mqtt
  {
    struct ClientConfig
    {
      std::string _address;
      std::string _clientID;
      auto to_string() const -> std::string;
    };

    struct ConnectConfig
    {
    };

    struct Payload
    {
      int size;
      void *bytes;
    };

    enum class Qos
    {
      e0 = 0,
      e1 = 1,
      e2 = 2
    };

    virtual ~Mqtt() = default;
    virtual auto InitClient() -> Result = 0;
    virtual auto DeInitClient() -> void = 0;
    virtual auto Connect(const ConnectConfig &) -> Result = 0;
    virtual auto Disconnect() -> Result = 0;
    virtual auto ReConnect() -> Result = 0;
    virtual auto IsConnected() -> bool = 0;
    virtual auto SendMessage(const std::string &topic, Qos qos, Payload payload) -> Result = 0;
    virtual auto Subscribe(const std::string &topic, Qos qos) -> Result = 0;
    virtual auto UnSubscribe(const std::string &topic) -> Result = 0;

    static auto RequestMqtt(const ClientConfig &) -> Mqtt *;
    static auto DumpVersionInfo() -> void;
  };

}