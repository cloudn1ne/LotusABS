#ifndef _WEB_H
#define _WEB_H

/*
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

extern ESP8266WebServer server;
extern WebSocketsServer ws_server;

// #include <WiFiClient.h>

#endif /* _WEB_H */