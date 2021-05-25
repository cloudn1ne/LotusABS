#ifndef _MQTT_H
#define _MQTT_H

#define MQTT_SERVER "10.146.248.37"
#define MQTT_USER "ha_mqtt"
#define MQTT_PASS "8aAluh17ALu1"
#define MQTT_PORT 1883

void mqtt_loop();
void mqtt_log(const char *prefix, const char *msg);
bool mqtt_setup();

#endif /* _MQTT_H */