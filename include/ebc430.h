#ifndef _EBC430_H
#define _EBC430_H

#include <stdint.h>

#define DBG_EBC430  1

// ReadECUIdentification Options
#define EBC_OPT_ALL           0x80
#define EBC_OPT_DESIGN_CODE   0x91
#define EBC_OPT_HARDWARE_VER  0x92
#define EBC_OPT_SOFTWARE_VER  0x94
#define EBC_OPT_PRODDATE      0x99

// ReadDataByLocalId Records
#define EBC_REC_VOLTAGE       0x30
#define EBC_REC_SPEED_FL      0x31  
#define EBC_REC_SPEED_FR      0x32
#define EBC_REC_SPEED_RL      0x33
#define EBC_REC_SPEED_RR      0x34


// list of callbacks
void ReadECUIdentificationCB(uint8_t *payload_bytes, uint8_t payload_len);
void ReadDataByLocalIdCB(uint8_t *payload_bytes, uint8_t payload_len);

// ReadECUIdentification Options variables
extern char ecu_id_designcode[12];     // Design Code A120J6000F_
extern char ecu_id_hardware[7];        // Hardware EBC430
extern char ecu_id_software[12];       // Software LCFWDQ04K2K
extern char ecu_id_date[10];           // Date of Production 03/02/25

// ReadDataByLocalId Records variables
extern float ecu_voltage;               // Battery Voltage
extern float speed_fl;                  // Front Left Wheel kph
extern float speed_fr;                  // Front Left Wheel kph
extern float speed_rl;                  // Front Left Wheel kph
extern float speed_rr;                  // Front Left Wheel kph
#endif /* _EBC430_H */