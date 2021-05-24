#ifndef _WEB_H
#define _WEB_H


#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

static AsyncWebServer server(80);

#endif /* _WEB_H */