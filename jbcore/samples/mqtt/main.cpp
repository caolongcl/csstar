#include <iostream>
#include <thread>
#include <chrono>

#include <def.hpp>
#include <mqtt.hpp>

using namespace dsg;

int main(int, char **) {
  Mqtt::DumpVersion();

  std::atomic<bool> disconnect{false};

  auto pubMqtt = Mqtt::Request(Mqtt::Config{
      // ._address = "tcp://mqtt.eclipseprojects.io:1883",
      ._address = "tcp://192.168.146.125:1883",
      // ._address = "tcp://test.mosquitto.org:1883",
      ._clientID = "ExampleClientPub",
      ._connectLostCallback =
          [](const std::string &msg) {
            DSG_LOG("connect lost, " << msg);
          },
      ._messageArrivedCallback =
          [](const Mqtt ::Topic &topic, std::span<uint8_t> msg) {
            DSG_LOG("message arrived " << topic.to_string() << " "
                                       << std::string(msg.begin(), msg.end()));
          },
      ._deliveryCompleteCallback =
          [](const std::string &) {

          },
      ._connectCallback =
          [](bool connected, const std::string &msg) {
            DSG_LOG("connect " << std::boolalpha << connected << " " << msg);
          },
      ._disconnectCallback =
          [&disconnect](bool disconnected, const std::string &msg) {
            DSG_LOG("disconnect " << std::boolalpha << disconnected << " " << msg);
            disconnect.store(disconnected);
          },
      ._sendCallback =
          [](bool success, const Mqtt::Topic &topic, const std::string &msg) {
            DSG_LOG("send " << topic.to_string() << " " << std::boolalpha << success << " " << msg);
          },
      ._subscribeCallback =
          [](bool success, const Mqtt::Topic &, const std::string &msg) {
            DSG_LOG("subscribe " << std::boolalpha << success << " " << msg);
          },
      ._unsubscribeCallback =
          [](bool success, const Mqtt::Topic &, const std::string &msg) {
            DSG_LOG("unsubscribe " << std::boolalpha << success << " " << msg);
          }});
  DSG_LOG("Usage:\n"
          "\tq # 退出\n"
          "\tc # 启动连接\n"
          "\tdc # 断开连接\n"
          "\tsub <topic> <qos> # 订阅 qos 0 1 2\n"
          "\tunsub <topic> <qos> # 取消订阅 qos 0 1 2\n"
          "\tsend <topic> <qos> <msg># 发送消息 qos 0 1 2\n");
  std::string input;
  std::stringstream ss;
  std::string item;
  std::vector<std::string> tokens;
  do {
    std::cout << "# ";
    ss.clear();
    tokens.clear();

    std::getline(std::cin, input);
    auto str = trim(input);
    ss << str;
    while (std::getline(ss, item, ' ')) {
      if (!item.empty()) {
        tokens.push_back(item);
      }
    }
    if (!tokens.empty()) {
      const auto &cmd = tokens[0];
      if (cmd == "q") {
        break;
      } else if (cmd == "c") {
        pubMqtt->Connect();
      } else if (cmd == "dc") {
        pubMqtt->Disconnect();
      } else if (cmd == "send") {
        if (tokens.size() == 4) {
          const auto &topic = tokens[1];
          const auto &qos = Mqtt::ToQos(std::stoi(tokens[2]));
          const auto &msg = tokens[3];
          pubMqtt->Send({topic, qos}, msg);
        }
      } else if (cmd == "sub") {
        if (tokens.size() == 3) {
          const auto &topic = tokens[1];
          const auto &qos = Mqtt::ToQos(std::stoi(tokens[2]));
          pubMqtt->Subscribe({topic, qos});
        }
      } else if (cmd == "unsub") {
        if (tokens.size() == 3) {
          const auto &topic = tokens[1];
          const auto &qos = Mqtt::ToQos(std::stoi(tokens[2]));
          pubMqtt->Unsubscribe({topic, qos});
        }
      }
    }
  } while (true);

  if (pubMqtt->IsConnected()) {
    pubMqtt->Disconnect();
  }
  return 0;
}
