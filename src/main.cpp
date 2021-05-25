#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FS.h>     
#include <TaskScheduler.h>
#include <stdint.h>
#include <LittleFS.h>

#include "web.h"
#include "kwp.h"
#include "mqtt.h"
#include "log.h"
#include "ebc430.h"

// #define   DBG_TASKS

#define TX 1
#define RX 3
#define LED_OK  16
KWP2K ABS(TX, RX);

AsyncWebServer server(80);

Scheduler Taskrunner;
void OKLEDTaskCallback();
Task OKLEDTask(1000, -1, &OKLEDTaskCallback);

bool state_ok_led = true;

uint8_t ecu_addr_glob = 0x1;


void setup() {

  // Taskrunner
  Taskrunner.init();
  Taskrunner.addTask(OKLEDTask);
  
  // Setup Serial
  Serial.begin(10400);
  
  // SETUP WiFI  
  WiFi.mode(WIFI_STA);  
  WiFi.begin("CLOUD9", "deepspace48");  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  mqtt_setup();
  log_msg("\n===================================\nLotusABS IP address is: %d.%d.%d.%d\n", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  
  // SETUP WWW
  if (!LittleFS.begin())
  {
    Serial.println("unable to init LittleFS");
  }
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
  server.on("/StartCommunication", HTTP_GET, [](AsyncWebServerRequest *request) {                     
      ABS.StartCommunication();
      request->send(200, "application/json", "{ \"status\" : true }");
  });
  server.on("/ReadECUIdentification", HTTP_GET, [](AsyncWebServerRequest *request) {          
      uint8_t option = EBC_OPT_ALL;
      if (request->hasArg("option"))
      {
          option = request->arg("option").toInt();                                       
      }
      ABS.ReadECUIdentification(option);
      request->send(200, "application/json", "{ \"status\" : true }");
  });
  server.on("/ReadDataByLocalId", HTTP_GET, [](AsyncWebServerRequest *request) {          
      uint8_t record;
      if (request->hasArg("record"))
      {
          record = request->arg("record").toInt();                                       
          ABS.ReadDataByLocalId(record);
      }      
      request->send(200, "application/json", "{ \"status\" : true }");
  });

  AsyncElegantOTA.begin(&server);
  server.begin();

  // ABS callbacks
  ABS.setCallback(KWP_SID_READECUIDENTIFICATION, &ReadECUIdentificationCB);
  ABS.setCallback(KWP_SID_READDATABYLOCALID, &ReadDataByLocalIdCB);
  


  // SETUP Pins
  pinMode(LED_OK, OUTPUT);  
  
  // enable Tasks
  OKLEDTask.enable();  
}

void OKLEDTaskCallback()
{
#ifdef DBG_TASKS
  log_msg("OKLEDTaskCallback()\n");
#endif  
  digitalWrite(LED_OK, state_ok_led);
  state_ok_led = !state_ok_led;
}

void loop() {

  mqtt_loop();
  ABS.process();
  AsyncElegantOTA.loop();
  Taskrunner.execute();
}

