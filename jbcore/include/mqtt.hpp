#pragma once

#include <functional>
#include <string>
#include <span>
#include "ptr.hpp"
#include "result.hpp"

namespace dsg {
struct Mqtt {
  enum class Qos : int { e0 = 0, e1 = 1, e2 = 2 };

  struct Topic {
    std::string _name{};
    Qos _qos{Qos::e0};

    auto to_string() const -> std::string;
  };

  static auto ToQos(int v) -> Mqtt::Qos {
    assert(v == 0 || v == 1 || v == 3);
    return v == 0 ? Mqtt::Qos::e0 : (v == 1 ? Mqtt::Qos::e1 : Mqtt::Qos::e2);
  }

  static auto FromQos(Mqtt::Qos v) -> int {
    return std::underlying_type_t<Mqtt::Qos>(v);
  }

  struct Config {
    std::string _address;
    std::string _clientID;

    // connect
    std::string _username{};
    std::string _pwd{};
    int _keepAliveInterval{60}; // seconds
    bool _cleansession{true};
    int _maxInflight{65535};
    int _connectTimeout{30};
    bool _automaticReconnect{false};
    int _minRetryInterval{1};  // seconds
    int _maxRetryInterval{60}; // seconds

    std::function<void(const std::string &)> _connectLostCallback{};
    std::function<void(const Topic &, std::span<uint8_t>)> _messageArrivedCallback{};
    std::function<void(const std::string &)> _deliveryCompleteCallback{};

    std::function<void(bool, const std::string &)> _connectCallback{};
    std::function<void(bool, const std::string &)> _disconnectCallback{};

    std::function<void(bool, const Topic &, const std::string &)> _sendCallback{};
    std::function<void(bool, const Topic &, const std::string &)> _subscribeCallback{};
    std::function<void(bool, const Topic &, const std::string &)> _unsubscribeCallback{};

    auto to_string() const -> std::string;
  };

  virtual ~Mqtt() = default;
  virtual auto Connect() -> Result = 0;
  virtual auto Disconnect() -> Result = 0;
  virtual auto ReConnect() -> Result = 0;
  virtual auto IsConnected() -> bool = 0;
  virtual auto Send(const Topic &topic, const std::string &textMsg) -> Result = 0;
  virtual auto Send(const Topic &topic, const std::vector<uint8_t> &payload) -> Result = 0;
  virtual auto Subscribe(const Topic &topic) -> Result = 0;
  virtual auto Unsubscribe(const Topic &topic) -> Result = 0;

  static auto Request(const Config &) -> interface_ptr<Mqtt>;
  static auto DumpVersion() -> void;
};

} // namespace dsg