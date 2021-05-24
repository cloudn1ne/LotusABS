#ifndef _LOG_H
#define _LOG_H

static bool log_serial_enabled = true;

void log_msg(const char *fmt, ...);

#endif /* _LOG_H */