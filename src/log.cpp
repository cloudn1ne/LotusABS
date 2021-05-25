/*
 * EBC430 ABS ECU
 * (c) Georg Swoboda, 2021
 *
 * >> Loggingfunctions <<
 */

#include <Arduino.h>
#include <stdint.h>
#include "mqtt.h"
#include "log.h"

bool log_serial_enabled = false;
bool log_mqtt_enabled = true;


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