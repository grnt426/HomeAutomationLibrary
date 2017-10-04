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
  _bypassWait = 0;
  _doNotSubscribe = 0;
  
  client = new PubSubClient(handler->espClient);
  client->setServer(_mqttServer, 1883);
  client->setCallback(callback);
}

/**
 * For some devices, like the AC, it is OK to wait longer than needed for
 * the wifi connection to settle before attempting the first MQTT connection.
 * For other devices, such as the ambient sensor, the connection to MQTT should
 * be attempted as soon as the wifi comes up.
 */
void MqttClient::bypassWait(int bypassWait) {
  _bypassWait = bypassWait;
}

/**
 * The ambient sensor does not care, nor will ever be awake, to receive messages
 * from any client or server. Subscribing will only increase the amount of
 * unnecessary network traffic on a battery operated device.
 */
void MqttClient::doNotSubscribe(int doNotSubscribe) {
  _doNotSubscribe = doNotSubscribe;
}

int MqttClient::reconnect() {
  if (_bypassWait || timerMan.isTimerPassed(mqttRetryT)) {

    Serial.print("Attempting MQTT connection...");

    // Unfortunately, the below call within the if-statement is blocking. As the
    // server is on the local network, timeouts are fast if the host is on, but
    // not listening on that port. Will test host off to see how long it takes.
    if (client->connect(deviceId)) {
      Serial.println("connected");

      if(_doNotSubscribe == 0) {
        client->subscribe(deviceIdTopic);
        client->subscribe(overwriteDeviceStateTopic);
      }

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
