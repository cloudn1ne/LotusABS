#ifndef _LOG_H
#define _LOG_H

extern bool log_serial_enabled;
extern bool log_mqtt_enabled;


void log_msg(const char *fmt, ...);

#endif /* _LOG_H */