#include <iostream>
#include <thread>
#include <chrono>

#include <mqtt.hpp>

using namespace DSG;

int main(int, char **)
{
    auto *mqtt = Mqtt::RequestMqtt(Mqtt::Config{
        // ._address = "tcp://mqtt.eclipseprojects.io:1883",
        ._address = "tcp://192.168.146.125:1883",
        // ._address = "tcp://test.mosquitto.org:1883",
        ._clientID = "ExampleClientPub"});
    mqtt->Connect({});
    std::this_thread::sleep_for(std::chrono::seconds(5));
    mqtt->Disconnect();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    delete mqtt;
    return 0;
}
