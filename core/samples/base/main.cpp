#include <iostream>
#include <thread>
#include <chrono>

#include <mqtt.hpp>

using namespace DSG;

#define DSG_TAG "    MAIN"

int main(int, char **)
{   
    Mqtt::DumpVersionInfo();
    
    std::string topic = "MqttTest";

    auto *subMqtt = Mqtt::RequestMqtt(Mqtt::ClientConfig{
        // ._address = "tcp://mqtt.eclipseprojects.io:1883",
        ._address = "tcp://192.168.146.125:1883",
        // ._address = "tcp://test.mosquitto.org:1883",
        ._clientID = "ExampleClientSub"});
    

    auto subThread = std::thread([subMqtt, &topic]() {
        if (subMqtt->Connect({}))
        {   
            std::this_thread::sleep_for(std::chrono::seconds(10));
            subMqtt->Subscribe(topic, Mqtt::Qos::e1); 
            std::this_thread::sleep_for(std::chrono::seconds(10));
            subMqtt->Disconnect();
        }  
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    auto *pubMqtt = Mqtt::RequestMqtt(Mqtt::ClientConfig{
        // ._address = "tcp://mqtt.eclipseprojects.io:1883",
        ._address = "tcp://192.168.146.125:1883",
        // ._address = "tcp://test.mosquitto.org:1883",
        ._clientID = "ExampleClientPub"});
    

    auto pubThread = std::thread([pubMqtt, &topic]() {
        if (pubMqtt->Connect({}))
        {   
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::string payload = "Hello World!";
            pubMqtt->SendMessage(topic, Mqtt::Qos::e1, {.size = static_cast<int>(payload.length()), .bytes = const_cast<char *>(payload.c_str())});
            std::this_thread::sleep_for(std::chrono::seconds(5));
            pubMqtt->Disconnect();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }  
    });

    subThread.join();
    pubThread.join();
    delete pubMqtt;
    delete subMqtt;
    return 0;
}

// [DSG D 1709898769598] Mqtt::ClientConfig{address:tcp://192.168.146.125:1883, client_id:ExampleClientSub}
// [DSG D 1709898769797] OnConnect:MQTTAsync_successData:{token:0, ver:4, sessionPresent:0, serverURI:192.168.146.125:1883}

// [DSG D 1709898779606] Mqtt::ClientConfig{address:tcp://192.168.146.125:1883, client_id:ExampleClientPub}
// [DSG D 1709898779607] Subscribing to topic MqttTest for client ExampleClientSub using QoS 1
// [DSG D 1709898779732] OnSubscribe:MQTTAsync_successData:{token:1, qos:1}
// [DSG D 1709898779785] OnConnect:MQTTAsync_successData:{token:0, ver:4, sessionPresent:0, serverURI:192.168.146.125:1883}

// [DSG D 1709898784669] DeliveryComplete token:1
// [DSG D 1709898784669] onSend:MQTTAsync_successData:{token:1, dest:MqttTest}
// [DSG D 1709898784671] MessageArrived: topic:MqttTest, msg:12, Hello World!

// [DSG D 1709898789613] OnDisconnect:MQTTAsync_successData:{token:0, ver:0, sessionPresent:0}

// [DSG D 1709898839612] OnDisconnect:MQTTAsync_successData:{token:0, ver:0, sessionPresent:0}
