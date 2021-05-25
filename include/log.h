#ifndef _LOG_H
#define _LOG_H

static bool log_serial_enabled = false;
static bool log_mqtt_enabled = true;


void log_msg(const char *fmt, ...);

#endif /* _LOG_H */