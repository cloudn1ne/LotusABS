#ifndef _WEB_H
#define _WEB_H


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// #define DBG_WEBSOCKETS 1
extern ESP8266WebServer server;
extern WebSocketsServer ws_server;

#endif /* _WEB_H */