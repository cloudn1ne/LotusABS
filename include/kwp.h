
#ifndef _KWP_H
#define _KWP_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdint.h>

#define DBG_KWP2K  1

#define KWP_SID_STARTCOMMUNICATION  0x81

#define KWP2K_DEFAULT_TESTERADDRESS 0xF1
#define KWP2k_DEFAULT_ECUADDRESS    0x20

#define KWP2K_BAUDRATE              10400

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


class KWP2KMsg
{
    public:
        KWP2KMsg(void);
        void setFmt(uint8_t header_mode, uint8_t datafield_len);
        void setSrc(uint8_t addr);
        void setTgt(uint8_t addr);
        void setSId(uint8_t sid);
        uint8_t getCS(void);
        void print(void);

    private:
        KWP2KMessageStruct _msg;
};


class KWP2K
{
    public:
        KWP2K(uint8_t tx_pin, uint8_t rx_pin);
        KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress);
        KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress, uint8_t Testeraddress);
        void setECUaddress(uint8_t ECUaddress);
        void setTesteraddress(uint8_t Testeraddress);
        void fastInit();
        void StartCommunication(void);
        void process();

    private:
        // Serial Port
        SoftwareSerial *_ser;
        uint8_t _tx_pin;
        uint8_t _rx_pin;

        // ECU Addressing
        uint8_t _ECUaddress;
        uint8_t _Testeraddress;      

        // Default Messages
        KWP2KMsg _msg_StartCommunication;
        
};



#endif /* _KWP_H */