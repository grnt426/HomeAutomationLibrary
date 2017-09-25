#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "WifiHandler.h"
#include "MqttClient.h"
#include "TimerManager.h"

PubSubClient* client;
TimerManager timerMan;

long lastMsg = 0;
int wifiOrigInit = 0;

int firstConn = 1;

int mqttRetryT = timerMan.registerTimer(15000);

MqttClient::MqttClient(const char* dId, void (*callback)(char*, byte*, unsigned int length), WifiHandler* handler, char* server) {
  deviceId = dId;
  _mqttServer = server;
  _wifiHandler = handler;
  
  deviceIdTopic = "ac/ac_beta";
  deviceSyncTopic = "ac/sync/ac_beta";
  overwriteDeviceStateTopic = "ac/overwrite/ac_beta";
  
  client = new PubSubClient(handler->espClient);
  client->setServer(_mqttServer, 1883);
  client->setCallback(callback);
}

/**
   Used to reconnect to the MQTT server, *NOT* the WiFi.
*/
int MqttClient::reconnect() {
  if (timerMan.isTimerPassed(mqttRetryT)) {

    Serial.print("Attempting MQTT connection...");

    // Unfortunately, the below call within the if-statement is blocking. As the
    // server is on the local network, timeouts are fast if the host is on, but
    // not listening on that port. Will test host off to see how long it takes.
    if (client->connect(deviceId)) {
      Serial.println("connected");
      client->subscribe(deviceIdTopic);
      client->subscribe(overwriteDeviceStateTopic);

      if (firstConn) {
        Serial.println("First MQTT connection, activating");
        firstConn = 0;
        snprintf (_msg, 75, "name:%s", deviceId);
        client->publish("activate", _msg);
        return 3;
      }
      else {
        Serial.println("Telling server we reconnected to MQTT");
        snprintf (_msg, 75, "name:%s", deviceId);
        client->publish("reconnect", _msg);
        return 2;
      }
    }
    else {
      Serial.print("failed, rc = ");
      Serial.print(client->state());
      Serial.println(" trying again in 15 seconds");
      timerMan.resetTimer(mqttRetryT);
      return -1;
    }
  }

  return 0;
}

int MqttClient::loop() {

  if (_wifiHandler->isWifiConnected()) {
    if (!client->connected()) {
      return reconnect();
    }
    else {
      client->loop();
      return 1;
    }
  }

  return 0;
}

void MqttClient::publishMessage(const char* topic, const char* msg) {
  client->publish(topic, msg);
}
