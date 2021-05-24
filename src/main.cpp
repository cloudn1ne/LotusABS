#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FS.h>     
#include <TaskScheduler.h>
#include <stdint.h>

#include "web.h"
#include "kwp.h"
#include "log.h"

// #define   DBG_TASKS

#define TX 1
#define RX 3
#define LED_OK  16
// SoftwareSerial ECU(3,1);
KWP2K ABS(3,1);

Scheduler Taskrunner;
void OKLEDTaskCallback();
Task OKLEDTask(1000, -1, &OKLEDTaskCallback);
bool state_ok_led = true;


int messagelength = 5;
byte message[5] = {
// FMT  TGT  SRC  SID  CRC
  0x81,0x12,0xF1,0x81,0x05
};

/*
void sendrequest() {
  Serial.println("send request");
  for (int i = 0; i < messagelength ; i++) {
    ECU.write(message[i]);
    Serial.print("sending data: ");
    Serial.println(message[i], HEX);
  } 
  Serial.println("request sent");
}

void receivereply()
{
  Serial.print("RX: ");
  if (ECU.available()) {
      byte inByte = ECU.read();      
      Serial.print(inByte,HEX);
      Serial.print(" ");
  }
  Serial.println();
}
     
*/

void setup() {

  // Taskrunner
  Taskrunner.init();
  Taskrunner.addTask(OKLEDTask);

  // Setup Serial
  Serial.begin(10400);
  
  // SETUP WiFI  
  WiFi.mode(WIFI_STA);
  int status = WL_IDLE_STATUS;
  status = WiFi.begin("CLOUD9", "deepspace48");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // SETUP WWW
  if (!SPIFFS.begin())
  {
    Serial.println("unable to init SPIFFS");
  }
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  server.on("/StartCommunication", HTTP_GET, [](AsyncWebServerRequest *request) {    
      ABS.fastInit();
      ABS.StartCommunication();
      request->send(200, "application/json", "{ \"status\" : true }");
  });

  AsyncElegantOTA.begin(&server);
  server.begin();

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

  ABS.process();
  AsyncElegantOTA.loop();
  Taskrunner.execute();
}