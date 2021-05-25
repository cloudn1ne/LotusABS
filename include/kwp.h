
#ifndef _KWP_H
#define _KWP_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdint.h>

#define DBG_KWP2K  1

#define KWP_SID_STARTCOMMUNICATION          0x81

#define KWP_SID_READECUIDENTIFICATION       0x1A
#define KWP_SID_READECUIDENTIFICATION_POS   0x1A+0x40

#define KWP_SID_READDATABYLOCALID           0x21 
#define KWP_SID_READDATABYLOCALID_POS       0x21+0x40

#define KWP2K_DEFAULT_TESTERADDRESS 0xF1
#define KWP2k_DEFAULT_ECUADDRESS    0x2B

#define KWP2K_BAUDRATE              10400
#define KWP2K_TIMEOUT               3000        // after 3sec of silence we need to reinitialize

// header modes
#define KWPMSG_HM2                  2       // Header with address information, physical target address

typedef struct KWP2KMessageStruct {
  uint8_t Fmt;
  uint8_t Tgt;
  uint8_t Src;
  uint8_t Len;
  uint8_t SId;
  uint8_t Data[255];
  uint8_t CS;
} KWP2KMessageStruct;

enum KWP2KMessageState { IDLE, QUEUED, SENT, RESPONSE_RECEIVED };

class KWP2KMsg
{
    public:
        KWP2KMsg(void);
        void setFmt(uint8_t header_mode, uint8_t datafield_len);        
        void setSrc(uint8_t addr);
        void setTgt(uint8_t addr);
        void setSId(uint8_t sid);
        void setData(uint8_t idx, uint8_t value);
        uint8_t getCS(void);
        uint8_t calcCS(void);
        void print(void);
        uint8_t getSId(void);
        uint8_t *getBytes(void);
        uint8_t getBytesLen(void);
        uint8_t *getPayloadBytes(void);
        uint8_t getPayloadBytesLen(void);        
        void setState(KWP2KMessageState state);
        KWP2KMessageState getState();
        void rxByte(uint8_t byte);
        void reset(void);

    private:        
        KWP2KMessageStruct _msg;
        KWP2KMessageState _state;

        uint8_t _idx;
        bool _rx_completed;
        
};


class KWP2K
{
    public:
        KWP2K(uint8_t tx_pin, uint8_t rx_pin);
        KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress);
        KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress, uint8_t Testeraddress);
        void setECUaddress(uint8_t ECUaddress);
        void setTesteraddress(uint8_t Testeraddress);        
        void StartCommunication(void);
        void ReadECUIdentification(uint8_t option);
        void ReadDataByLocalId(uint8_t record);
        void process(void);          
        void setCallback(uint8_t SId, void (*callback)(uint8_t *payload_bytes, uint8_t payload_len));

    private:
        void fastInit(void);
        void resetFlags(void);
        void sendMsg(KWP2KMsg msg);
            
        void (*cb_ReadECUIdentification)(uint8_t *payload_bytes, uint8_t payload_len);
        void (*cb_ReadDataByLocalId)(uint8_t *payload_bytes, uint8_t payload_len);

        // Serial Port
        SoftwareSerial *_ser;
        uint8_t _tx_pin;
        uint8_t _rx_pin;

        // ECU Addressing
        uint8_t _ECUaddress;
        uint8_t _Testeraddress;      

        // Message to be sent
        KWP2KMsg _msgtx;

        // Message received
        KWP2KMsg _msgrx;

        // fast init, slow init established ?
        bool _is_initalized; 
        uint32_t _last_msg_ts;                        
};



#endif /* _KWP_H */