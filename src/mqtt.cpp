/*
 * Lotus ABS Tool
 * (c) Georg Swoboda, 2021
 *
 * >> MQTT <<
 */

// #include "intex.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <stdint.h>
#include "mqtt.h"
#include "log.h"

static bool mqtt_enabled = false;
long mqtt_lastreconnectattempt = 0;

static WiFiClient wifiClient;
static PubSubClient mqtt_client(wifiClient);

// internal periodic mqtt loop handler for PubSubClient
// handles reconnecting to MQTT if connection is lost
void mqtt_loop()
{   

    if (mqtt_enabled)
    {
        if (!mqtt_client.connected())        // reconnect mqtt client after some delay
        {            
            long now = millis();
            if (now - mqtt_lastreconnectattempt > 10000) // 10sec delay
            {
                mqtt_lastreconnectattempt = now;
                // Attempt to reconnect      
                log_msg("mqtt_loop(): mqtt reconnecting...\n");
                mqtt_setup();                   
            }
        } else 
        {            
            mqtt_client.loop();             // mqtt_client connected run PubSub Handler
        }    
    }
}

// logging via mqtt
void mqtt_log(const char *prefix, const char *msg)
{    
    char topic[64];
    
    sprintf(topic, "lotusabs/log/%s", prefix);                    
    mqtt_client.publish(topic, msg);
}

// setup mqtt and connect to server
bool mqtt_setup()
{    
    log_msg("mqtt_setup()\n");
    if (log_mqtt_enabled)
    {   
        String mqtt_server = MQTT_SERVER;
        String mqtt_user = MQTT_USER;
        String mqtt_pass = MQTT_PASS;
        unsigned int mqtt_port = MQTT_PORT;

        log_msg("mqtt_setup()\n");
        log_msg("  mqtt_server: %s:%d\n", mqtt_server.c_str(), mqtt_port);
        log_msg("  mqtt_user: '%s'\n", mqtt_user.c_str());
        log_msg("  mqtt_pass: '%s'\n\n", mqtt_pass.c_str());

        mqtt_client.setServer(mqtt_server.c_str(), mqtt_port);
        mqtt_client.connect("lotusabs", mqtt_user.c_str(), mqtt_pass.c_str());              
        if (mqtt_client.connected()) 
        {
            log_msg("mqtt_setup(): connected !\n");            
            // mqtt_client.setCallback(mqtt_callback);        
            // mqtt_client.subscribe("purewifi/set_temp/set");
            mqtt_enabled = true;  // <- global MQTT enable flag
        }        
        else
        {
            log_msg("mqtt_setup(): unable to connect ! \n");
        }
    }    
    return(mqtt_enabled);
}