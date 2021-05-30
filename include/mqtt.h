#ifndef _MQTT_H
#define _MQTT_H

// here you can define a MQTT server that will get debug output logs (if enabled in log.h)
#define MQTT_SERVER "1.2.3.4"
#define MQTT_USER "mqtt"
#define MQTT_PASS "yeahsure"
#define MQTT_PORT 1883

void mqtt_loop();
void mqtt_log(const char *prefix, const char *msg);
bool mqtt_setup();

#endif /* _MQTT_H */