/*
 * EBC430 ABS ECU
 * (c) Georg Swoboda, 2021
 *
 * >> Loggingfunctions <<
 */

#include <Arduino.h>
#include <stdint.h>
#include "log.h"

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
}