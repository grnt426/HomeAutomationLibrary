#include <ESP8266WiFi.h>
#include "WifiHandler.h"

WifiHandler::WifiHandler(const char* ssid, const char* password) {
  _ssid = ssid;
  _password = password;
}

int WifiHandler::loop() {

  if (_wifiOrigInit == 0 && millis() > 10000) {
    _wifiOrigInit = 1;

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(_ssid);

    WiFi.begin(_ssid, _password);
  }
  else {
    if (_wifiFinallyConn == 0 && WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      _wifiState = 1;
      _wifiFinallyConn = 1;
      return 1;
    }
    if(_wifiFinallyConn == 1) {
      return 1;
    }
  }
  return 0;
}

int WifiHandler::isWifiConnected() {
  return _wifiFinallyConn;
}
