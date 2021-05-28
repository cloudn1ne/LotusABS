
#ifndef _KWP_H
#define _KWP_H
#include <list>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdint.h>


#define DBG_KWP2K  1

#define KWP_SID_STARTCOMMUNICATION          0x81
#define KWP_SID_STARTCOMMUNICATION_POS      0x81+0x40

#define KWP_SID_READECUIDENTIFICATION       0x1A
#define KWP_SID_READECUIDENTIFICATION_POS   0x1A+0x40

#define KWP_SID_READDATABYLOCALID           0x21 
#define KWP_SID_READDATABYLOCALID_POS       0x21+0x40

#define KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS      0x18 
#define KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS_POS  0x18+0x40

#define KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE       0x14
#define KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE_POS   0x14+0x40

#define KWP_SID_ERR                         0x7F

#define KWP2K_DEFAULT_TESTERADDRESS 0xF1
#define KWP2k_DEFAULT_ECUADDRESS    0x2B

#define KWP2K_BAUDRATE              10400
#define KWP2K_SESSION_TIMEOUT       3000        // after 3sec of silence we need to reinitialize

#define KWP2K_REPLY_TIMEOUT         2000        // after 2sec of not receiving a reply, we give up

#define KWP2K_INTERMSG_DELAY        25          // enforced delay between sending a message after receiving one

// header modes
#define KWPMSG_HM2                  2           // Header with address information, physical target address

// TX Queue size
#define KWP2K_TX_QUEUE_SIZE         10


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
        KWP2KMsg *getStartCommunicationMsg();
        void process(void);          
        void setCallback(uint8_t SId, void (*callback)(uint8_t *payload_bytes, uint8_t payload_len));
        void setTimeoutCallback(void (*callback)(void));
        // ECU Functions
        void StartCommunication(void);
        void ReadECUIdentification(uint8_t option);
        void ReadDataByLocalId(uint8_t record);        
        void ReadDiagnosticTroubleCodesByStatus(void);
        void ClearDiagnosticInformationService(void);
        

    private:
        void fastInit(void);
        void resetFlags(void);
        void sendMsg(KWP2KMsg *msg);
        bool pushTXQueue(KWP2KMsg *msg);
        KWP2KMsg *popTXQueue(void);
        KWP2KMsg *peekTXQueue(void);

        void (*cb_Timeout)(void);                            
        void (*cb_ErrorMessage)(uint8_t *payload_bytes, uint8_t payload_len);        
        void (*cb_StartCommunication)(uint8_t *payload_bytes, uint8_t payload_len);
        void (*cb_ReadECUIdentification)(uint8_t *payload_bytes, uint8_t payload_len);
        void (*cb_ReadDataByLocalId)(uint8_t *payload_bytes, uint8_t payload_len);
        void (*cb_ReadDiagnosticTroubleCodesByStatus)(uint8_t *payload_bytes, uint8_t payload_len);
        void (*cb_ClearDiagnosticInformationService)(uint8_t *payload_bytes, uint8_t payload_len);
        

        // Serial Port
        SoftwareSerial *_ser;
        uint8_t _tx_pin;
        uint8_t _rx_pin;

        // ECU Addressing
        uint8_t _ECUaddress;
        uint8_t _Testeraddress;      

        // Message to be sent        
        KWP2KMsg *_tx_queue[KWP2K_TX_QUEUE_SIZE];
        bool _waiting_for_reply;
        uint32_t _waiting_for_reply_timeout;


        //KWP2KMsg _msgtx;

        // Buffer for receiving messages
        KWP2KMsg _msgrx;

        // fast init, slow init established ?
        bool _is_initialized; 
        uint32_t _last_tx_msg_ts;
        uint32_t _last_rx_msg_ts;                        

        // CommunicationStarted (Session still alive)
        bool _is_communication_started;        
};



#endif /* _KWP_H */