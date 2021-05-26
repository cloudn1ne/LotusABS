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

// AsyncWebServer server(80);
ESP8266WebServer server(80);

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
  server.serveStatic("/", LittleFS, "/www/index.html");
  server.serveStatic("/js", LittleFS, "/www/js");
  server.serveStatic("/img", LittleFS, "/www/img");
  server.serveStatic("/css", LittleFS, "/www/css");

/*
  server.on("/StartCommunication", HTTP_GET, []() {                     
      ABS.StartCommunication();    
      server.send(200, "application/json", "{ \"status\" : true }");
  });  
  */
  server.on("/ReadECUIdentification", HTTP_GET, []() {            
      uint8_t option = EBC_OPT_ALL;
      if (server.hasArg("option"))
      {
          option = server.arg("option").toInt();                                       
         
      }  
      ABS.ReadECUIdentification(option);          
      server.send(200, "application/json", "{ \"status\" : true }");
  });
  server.on("/ReadDataByLocalId", HTTP_GET, []() {            
      uint8_t record;
      if (server.hasArg("record"))
      {
          record = server.arg("record").toInt();                                       
          ABS.ReadDataByLocalId(record);
      }      
      server.send(200, "application/json", "{ \"status\" : true }");
  });
  server.on("/ReadDiagnosticTroubleCodesByStatus", HTTP_GET, []() {                  
      ABS.ReadDiagnosticTroubleCodesByStatus();   
      server.send(200, "application/json", "{ \"status\" : true }");
  });
  server.on("/ClearDiagnosticInformationService", HTTP_GET, []() {                  
      ABS.ClearDiagnosticInformationService();   
      server.send(200, "application/json", "{ \"status\" : true }");
  });
    
  // AsyncElegantOTA.begin(&server);
  ElegantOTA.begin(&server); 
  server.begin();

  // register ABS ECU callbacks
  ABS.setCallback(KWP_SID_READECUIDENTIFICATION, &ReadECUIdentificationCB);
  ABS.setCallback(KWP_SID_READDATABYLOCALID, &ReadDataByLocalIdCB);
  ABS.setCallback(KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS, &ReadDiagnosticTroubleCodeByStatusCB);
  ABS.setCallback(KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE, &ClearDiagnosticInformationServiceCB);
  
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
 //  AsyncElegantOTA.loop();
  Taskrunner.execute();
  server.handleClient();
}

