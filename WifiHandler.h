#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <ESP8266WiFi.h>

class WifiHandler {
  public:
    WifiHandler(const char* ssid, const char* password);
    int loop();
    int isWifiConnected();
    
    WiFiClient espClient;
  private:
    const char* _ssid;
    const char* _password;
    int _wifiOrigInit;
    int _wifiFinallyConn;
    int _wifiState;
};
#endif
