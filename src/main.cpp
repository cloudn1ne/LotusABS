#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FS.h>     
#include <TaskScheduler.h>
#include <stdint.h>
#include <LittleFS.h>
#include <ElegantOTA.h>

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


ESP8266WebServer server(80);
WebSocketsServer ws_server = WebSocketsServer(81);

Scheduler Taskrunner;
void OKLEDTaskCallback();
Task OKLEDTask(1000, -1, &OKLEDTaskCallback);

bool state_ok_led = true;

uint8_t ecu_addr_glob = 0x1;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{
    DynamicJsonDocument json(128);
    switch(type) {
        case WStype_DISCONNECTED:
#ifdef DBG_WEBSOCKETS                    
            log_msg("webSocketEvent() [%u] Disconnected!\n", num);
#endif            
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = ws_server.remoteIP(num);
#ifdef DBG_WEBSOCKETS                            
                log_msg("webSocketEvent() [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif                
      //          ws_server.sendTXT(num, "Connected");
            }
            break;
        case WStype_TEXT:        
#ifdef DBG_WEBSOCKETS                    
            log_msg("webSocketEvent() [%u] get Text: %s\n", num, payload);
#endif            
            deserializeJson(json, payload);       
#ifdef DBG_WEBSOCKETS            
            log_msg("\t Calling SId: %02x", json["sid"].as<int>());     
#endif            
            switch (json["sid"].as<int>())          
            {
                case KWP_SID_STARTCOMMUNICATION:                  ABS.StartCommunication(); break;
                case KWP_SID_READECUIDENTIFICATION:               ABS.ReadECUIdentification(json["option"].as<int>()); break;
                case KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS:  ABS.ReadDiagnosticTroubleCodesByStatus(); break;
                case KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE:   ABS.ClearDiagnosticInformationService(); break;
                case KWP_SID_READDATABYLOCALID:                   ABS.ReadDataByLocalId(json["record"].as<int>(), json["dtc"].as<int>()); break;
                case KWP_SID_INPUTOUTPUTCONTROLBYLOCALID:         ABS.InputOutputControlByLocalId(json["id"].as<int>(), json["ctrl_param"].as<int>(), json["ctrl_state"].as<int>()); break;
                default:
#ifdef DBG_WEBSOCKETS                            
                          log_msg("\n unknown SiD, no function defined ?");
#endif                           
                          break;
            }
            break;        
    }

}


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
  server.serveStatic("/valves.html", LittleFS, "/www/valves.html");
  server.serveStatic("/dtc.html", LittleFS, "/www/dtc.html");
  server.serveStatic("/js", LittleFS, "/www/js");
  server.serveStatic("/img", LittleFS, "/www/img");
  server.serveStatic("/css", LittleFS, "/www/css");

  server.on("/ReadECUIdentification", HTTP_GET, []() {            
      uint8_t option = EBC_OPT_ALL;
      if (server.hasArg("option"))
      {
          option = server.arg("option").toInt();                                       
         
      }  
      ABS.ReadECUIdentification(option);          
      server.send(200, "application/json", "{ \"status\" : true }");
  });


  // AsyncElegantOTA.begin(&server);
  ElegantOTA.begin(&server); 
  server.begin();

  // register ABS ECU callbacks
  ABS.setTimeoutCallback(&TimeoutCB);
  ABS.setCallback(KWP_SID_ERR, &ErrorMessageCB);
  ABS.setCallback(KWP_SID_STARTCOMMUNICATION, &StartCommunicationCB);
  ABS.setCallback(KWP_SID_READECUIDENTIFICATION, &ReadECUIdentificationCB);
  ABS.setCallback(KWP_SID_READDATABYLOCALID, &ReadDataByLocalIdCB);
  ABS.setCallback(KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS, &ReadDiagnosticTroubleCodeByStatusCB);
  ABS.setCallback(KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE, &ClearDiagnosticInformationServiceCB);
  ABS.setCallback(KWP_SID_INPUTOUTPUTCONTROLBYLOCALID, &InputOutputControlByLocalIdCB);

  // SETUP Pins
  pinMode(LED_OK, OUTPUT);  
  
  // enable Tasks
  OKLEDTask.enable();  

  // enable webSockets
    ws_server.begin();
    ws_server.onEvent(webSocketEvent);
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
  ws_server.loop();
}

