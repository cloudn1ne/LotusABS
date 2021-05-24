#include <Arduino.h>
#include "kwp.h"
#include "log.h"

// constructor and overloads
KWP2K::KWP2K(uint8_t tx_pin, uint8_t rx_pin)
{
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;
    _ECUaddress = KWP2k_DEFAULT_ECUADDRESS;
    _Testeraddress = KWP2K_DEFAULT_TESTERADDRESS;
    _ser =  new SoftwareSerial(_tx_pin, _rx_pin);
    _ser->begin(KWP2K_BAUDRATE);
}

KWP2K::KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress)
{
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;
    _ECUaddress = ECUaddress;
    _Testeraddress = KWP2K_DEFAULT_TESTERADDRESS;
    _ser =  new SoftwareSerial(_tx_pin, _rx_pin);
    _ser->begin(KWP2K_BAUDRATE);
}

KWP2K::KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress, uint8_t Testeraddress)
{
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;
    _ECUaddress = ECUaddress;
    _Testeraddress = Testeraddress;
    _ser =  new SoftwareSerial(_tx_pin, _rx_pin);
    _ser->begin(KWP2K_BAUDRATE);
}

// Proccess
void KWP2K::process()
{    
    uint8_t n=0;
    while (_ser->available())
    {
          uint8_t in = _ser->read();      
          n++;
          printf(" %02x", in);      
    }
    if (n)
        printf("\n");
    
}

// Fast Init
void KWP2K::fastInit()
{
#ifdef DBG_KWP2K
    log_msg("KWP2K::fastInit()\n");      
#endif     
    digitalWrite (_tx_pin, HIGH);  
    delay(500);             
    digitalWrite (_tx_pin, LOW);      
    delay(25);
    digitalWrite (_tx_pin, HIGH); 
    delay(25);                   
}

// functions
void KWP2K::StartCommunication()
{    
  _msg_StartCommunication.setFmt(KWPMSG_HM2, 1);  
  _msg_StartCommunication.setSrc(_Testeraddress);  
  _msg_StartCommunication.setTgt(_ECUaddress);
  _msg_StartCommunication.setSId(KWP_SID_STARTCOMMUNICATION);  
  _msg_StartCommunication.print(); 
}

// addressing
void KWP2K::setECUaddress(uint8_t ECUaddress)
{
    _ECUaddress = ECUaddress;
}
void KWP2K::setTesteraddress(uint8_t Testeraddress)
{
    _Testeraddress = Testeraddress;
}



// ===============================================
// ================= KWP2KMsg ====================
// ==============================================

KWP2KMsg::KWP2KMsg(void)
{
    memset(&_msg, 0, sizeof(KWP2KMessageStruct));
}

void KWP2KMsg::setFmt(uint8_t header_mode, uint8_t datafield_len)
{ 
#ifdef DBG_KWP2KMsg    
    log_msg("KWP2KMsg::setFmt(header_mode=%02x, datafield_len=%02x)\n", header_mode, datafield_len);      
#endif    
    header_mode = header_mode & 0x3;        // mask 2 bits   
    datafield_len = datafield_len & 0x3F;   // mask 6 bits
    _msg.Fmt = datafield_len | (header_mode<<6);    
}

void KWP2KMsg::setSrc(uint8_t addr)
{
#ifdef DBG_KWP2KMsg
    log_msg("KWP2KMsg::setSrc(src=%02x)\n", addr);
#endif    
    _msg.Src = addr;
}
void KWP2KMsg::setTgt(uint8_t addr)
{
#ifdef DBG_KWP2KMsg    
    log_msg("KWP2KMsg::setTgt(src=%02x)\n", addr);
#endif    
    _msg.Tgt = addr;
}

void KWP2KMsg::setSId(uint8_t sid)
{
#ifdef DBG_KWP2KMsg
    log_msg("KWP2KMsg::setSId(sid=%02x)\n", sid);
#endif    
    _msg.SId = sid;
}

uint8_t KWP2KMsg::getCS(void)
{
    uint8_t sum = 0;    
    sum = _msg.Fmt + _msg.Src + _msg.Tgt +_msg.SId;
    // TODO: add _msg.Data bytes to checksum
    return(sum);
}

void KWP2KMsg::print(void)
{
    log_msg("KWP2KMsg::print()\n");
    log_msg("\t Msg.Fmt: %02x\n", _msg.Fmt);    
    log_msg("\t Msg.Tgt: %02x\n", _msg.Tgt);    
    log_msg("\t Msg.Src: %02x\n", _msg.Src);        
    log_msg("\t Msg.SId: %02x\n", _msg.SId);    
    log_msg("\t Msg.CS:  %02x\n", this->getCS());
    log_msg("\n");
}