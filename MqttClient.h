#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H
#include "WifiHandler.h"

class MqttClient {
public:
  MqttClient(const char* deviceId, void (*callback)(char*, byte*, unsigned int length), WifiHandler* handler, char* server);
  int loop();
  void publishMessage(const char* topic, const char* msg);
  
  const char* deviceId;
  int wifiOrigInit;
  const char* mqttServer;
  const char* overwriteDeviceStateTopic;
  const char* deviceSyncTopic;
  const char* deviceIdTopic;

private:
  int reconnect();
  
  WifiHandler* _wifiHandler;
  char _msg[100];
  char* _mqttServer;
};
#endif