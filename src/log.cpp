/*
 * Lotus ABS ECU Tool
 * (c) Georg Swoboda, 2021
 *
 * >> Loggingfunctions <<
 */

#include <Arduino.h>
#include <stdint.h>
#include "mqtt.h"
#include "log.h"

bool log_serial_enabled = false;        // enable for serial output (as the RX/TX pins are shared with the kline you probably dont want this if hooked up to the car)
bool log_mqtt_enabled = false;          // enable this for MQTT based logging (see mqtt.h for server config)


// log message to all enabled sinks
void log_msg(const char *fmt, ...)
{
    char buffer[512];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (log_serial_enabled)
        Serial.print(buffer);          
    if (log_mqtt_enabled)
        mqtt_log("msg", buffer);            
}