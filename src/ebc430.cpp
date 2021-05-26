#include "log.h"
#include "kwp.h"
#include "ebc430.h"

char ecu_id_designcode[12];     // Design Code A120J6000F_
char ecu_id_hardware[7];        // Hardware EBC430
char ecu_id_software[12];       // Software LCFWDQ04K2K
char ecu_id_date[10];           // Date of Production 03/02/25
float ecu_voltage;              // Voltage
float speed_fl;                 // Front Left Wheel kph
float speed_fr;                 // Front Left Wheel kph
float speed_rl;                 // Front Left Wheel kph
float speed_rr;                 // Front Left Wheel kph

void ReadECUIdentificationCB(uint8_t *payload_bytes, uint8_t payload_len)
{        
#ifdef DBG_EBC430    
    log_msg("ReadECUIdentificationCB(option=%02x len=%d)\n", payload_bytes[1], payload_len);
#endif    

    uint8_t option = payload_bytes[1];
    switch (option)
    {
        case EBC_OPT_DESIGN_CODE:  strncpy(ecu_id_designcode, (char*)payload_bytes+2, payload_len-2); break;
        case EBC_OPT_HARDWARE_VER:  strncpy(ecu_id_hardware, (char*)payload_bytes+2, payload_len-2); break;
        case EBC_OPT_SOFTWARE_VER:  strncpy(ecu_id_software, (char*)payload_bytes+2, payload_len-2); break;
        case EBC_OPT_PRODDATE:  strncpy(ecu_id_date, (char*)payload_bytes+2, payload_len-2); break;
        case EBC_OPT_ALL:       strncpy(ecu_id_designcode, (char*)payload_bytes+2, 11);
                                strncpy(ecu_id_hardware, (char*)payload_bytes+13, 6);
                                strncpy(ecu_id_software, (char*)payload_bytes+19, 11);
                                strncpy(ecu_id_date, (char*)payload_bytes+30, 9);
                                break;
    }

#ifdef DBG_EBC430    
    log_msg("Design Code: '%s'\n", ecu_id_designcode);
    log_msg("Hardware Ver: '%s'\n", ecu_id_hardware);
    log_msg("Software Ver: '%s'\n", ecu_id_software);
    log_msg("Prod Date: '%s'\n", ecu_id_date);
#endif    
}


void ReadDataByLocalIdCB(uint8_t *payload_bytes, uint8_t payload_len)
{        
#ifdef DBG_EBC430    
    log_msg("ReadDataByLocalIdCB(record=%02x)\n", payload_bytes[1]);
#endif    
    uint8_t record = payload_bytes[1];
    switch (record)
    {
        case EBC_REC_VOLTAGE:   ecu_voltage = (float)(payload_bytes[2]*0.1); break;
        case EBC_REC_SPEED_FL:  speed_fl = (float)(((payload_bytes[2]<<8)+payload_bytes[1])/256); break;
        case EBC_REC_SPEED_FR:  speed_fr = (float)(((payload_bytes[2]<<8)+payload_bytes[1])/256); break;
        case EBC_REC_SPEED_RL:  speed_rl = (float)(((payload_bytes[2]<<8)+payload_bytes[1])/256); break;
        case EBC_REC_SPEED_RR:  speed_rr = (float)(((payload_bytes[2]<<8)+payload_bytes[1])/256); break;
    }
#ifdef DBG_EBC430    
    log_msg("Voltage: %2.1fV\n", ecu_voltage);    
    log_msg("Speeds Front: %3.1f kph   %3.1 kph\n", speed_fl, speed_fr);    
    log_msg(" Speeds Rear: %3.1f kph   %3.1 kph\n", speed_rl, speed_rr);    
#endif      
}

void ReadDiagnosticTroubleCodeByStatusCB(uint8_t *payload_bytes, uint8_t payload_len)
{        
#ifdef DBG_EBC430    
    log_msg("ReadDiagnosticTroubleCodeByStatus(num_of_dtcs=%d)\n", payload_bytes[1]);
#endif    
    uint8_t dtc_count = payload_bytes[1];
    uint8_t dtc_idx, dtc_type_code;
    char dtc_type_char;
    uint16_t dtc_code;
    uint8_t dtc_status;

    for (dtc_idx=0 ; dtc_idx<dtc_count ; dtc_idx++)
    {
        dtc_type_code = (payload_bytes[2+(3*dtc_idx)] >> 6)&0x3;       // the two MSB's encode the component type (P)owertrain, (C)hassis, (B)ody, (U)Network        
        switch (dtc_type_code)
        {
            case 0x0: dtc_type_char='P'; break;
            case 0x1: dtc_type_char='C'; break;
            case 0x2: dtc_type_char='B'; break;
            case 0x3: dtc_type_char='U'; break;
            default: dtc_type_char='?';
        }

        dtc_code = (payload_bytes[2+(3*dtc_idx)]<<8) | payload_bytes[2+(3*dtc_idx)+1];
        dtc_code &= 0x3FFF;    // mask off dtc type code bits        
        dtc_status = payload_bytes[2+(3*dtc_idx)+2];

#ifdef DBG_EBC430    
        log_msg("DTC(#%d) is %c%04x (status_byte=%02x)", dtc_idx+1,  dtc_type_char, dtc_code, dtc_status);
#endif        
    }
    
#ifdef DBG_EBC430    
    log_msg("Number of DTCs: %d\n", dtc_count);        
#endif      
}


void ClearDiagnosticInformationServiceCB(uint8_t *payload_bytes, uint8_t payload_len)
{        
#ifdef DBG_EBC430    
    log_msg("ClearDiagnosticInformationServiceCB()\n");    
#endif    
}