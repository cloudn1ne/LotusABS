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
    _ser =  new SoftwareSerial(_rx_pin, _tx_pin);
    _ser->begin(KWP2K_BAUDRATE);
    this->resetFlags();
}

KWP2K::KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress)
{
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;
    _ECUaddress = ECUaddress;
    _Testeraddress = KWP2K_DEFAULT_TESTERADDRESS;
    _ser =  new SoftwareSerial(_rx_pin, _tx_pin);
    _ser->begin(KWP2K_BAUDRATE);
    this->resetFlags();
}

KWP2K::KWP2K(uint8_t tx_pin, uint8_t rx_pin, uint8_t ECUaddress, uint8_t Testeraddress)
{
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;
    _ECUaddress = ECUaddress;
    _Testeraddress = Testeraddress;
    _ser =  new SoftwareSerial(_rx_pin, _tx_pin);
    _ser->begin(KWP2K_BAUDRATE);
    this->resetFlags();
}

void KWP2K::resetFlags()
{
    _is_initalized = false;
    _last_msg_ts = 0;           // never    
}

void KWP2K::setCallback(uint8_t SId, void (*callback)(uint8_t *payload_bytes, uint8_t payload_len))
{
    switch (SId)
    {
        case KWP_SID_READECUIDENTIFICATION: cb_ReadECUIdentification=callback;
        case KWP_SID_READDATABYLOCALID:     cb_ReadDataByLocalId=callback;
    }
}

//////////////////////////////////////////////
// Process KWP2K Handler
//////////////////////////////////////////////
void KWP2K::process()
{      
    // should we trigger a fastInit ?
    if (_last_msg_ts+KWP2K_TIMEOUT < millis())
    {
        _is_initalized = false; // force reinitialization 
    }        

    // is there a pending message that is to be sent ?
    if (_msgtx.getState() == QUEUED)
    {    
        if (!_is_initalized)
        {
            this->fastInit();
        }
        this->sendMsg(_msgtx);           

        // we expect to read back what we sent, and then any replies from the ECU
        uint8_t expected_readback_len = _msgtx.getBytesLen(); 
        uint8_t *expected_bytes = _msgtx.getBytes();
        for (uint8_t i=0;i<expected_readback_len;i++)
        {
            if (_ser->read() != expected_bytes[i])
            {
                log_msg("KWP2K::process() error reading back sent message\n");                
                break;
            }
        }      
        _msgtx.setState(SENT);
        free(expected_bytes);  
        _msgrx.reset();         // make ready the _msgrx buffer      
#ifdef DBG_KWP2K
        log_msg("KWP2K::process() message SENT\n");      
#endif             
    }

    // and now any reply from the ECU itself
    if (_msgtx.getState() == SENT)
    {               
        while (_ser->available())
        {
            _msgrx.setState(IDLE);
            uint8_t in = _ser->read();      
            _msgrx.rxByte(in);
        }   
        // log_msg("loop exit debug");    
    }    

    // CS was ok, so the response has been received
    if (_msgrx.getState() == RESPONSE_RECEIVED) 
    {
        log_msg("RESPONSE RECEIVED SUCCESSFULLY (%d)\n", _msgrx.getPayloadBytesLen());
        uint8_t i;
        uint8_t *payload = _msgrx.getPayloadBytes();
        
        for (i=0;i<_msgrx.getPayloadBytesLen();i++)        
        {
            log_msg("%02x ", payload[i]);

        }
        switch(_msgrx.getSId())
        {
            case KWP_SID_READECUIDENTIFICATION_POS: this->cb_ReadECUIdentification(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
            case KWP_SID_READDATABYLOCALID_POS: this->cb_ReadDataByLocalId(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
        }

        free(payload);
        _msgrx.setState(IDLE);
    }
}

// Fast Init
void KWP2K::fastInit()
{
#ifdef DBG_KWP2K
    log_msg("KWP2K::fastInit(tx_pin=%d)\n", _tx_pin);      
#endif     
    _ser->end();
    pinMode(_tx_pin, OUTPUT);    
    digitalWrite (_tx_pin, HIGH);  
    delay(500);             
    digitalWrite (_tx_pin, LOW);      
    delay(25);
    digitalWrite (_tx_pin, HIGH); 
    delay(25);   
    // we should be initialized now, for the fastInit there is no way to know for sure
    _is_initalized = true;
}

// Functions
void KWP2K::StartCommunication()
{        
  _msgtx.setFmt(KWPMSG_HM2, 1);  
  _msgtx.setSrc(_Testeraddress);  
  _msgtx.setTgt(_ECUaddress);
  _msgtx.setSId(KWP_SID_STARTCOMMUNICATION);  
  _msgtx.setState(QUEUED);
  _msgtx.print();     
}

void KWP2K::ReadECUIdentification(uint8_t option)
{        
  _msgtx.setFmt(KWPMSG_HM2, 2);  
  _msgtx.setSrc(_Testeraddress);  
  _msgtx.setTgt(_ECUaddress);
  _msgtx.setSId(KWP_SID_READECUIDENTIFICATION);  
  _msgtx.setData(0, option);
  _msgtx.setState(QUEUED);
  _msgtx.print();     
}

void KWP2K::ReadDataByLocalId(uint8_t record)
{        
  _msgtx.setFmt(KWPMSG_HM2, 2);  
  _msgtx.setSrc(_Testeraddress);  
  _msgtx.setTgt(_ECUaddress);
  _msgtx.setSId(KWP_SID_READDATABYLOCALID);  
  _msgtx.setData(0, record);  
  _msgtx.setState(QUEUED);
  _msgtx.print();     
}


// Addressing
void KWP2K::setECUaddress(uint8_t ECUaddress)
{
    _ECUaddress = ECUaddress;
}
void KWP2K::setTesteraddress(uint8_t Testeraddress)
{
    _Testeraddress = Testeraddress;
}

// Send a message 
void KWP2K::sendMsg(KWP2KMsg msg)
{       
    uint8_t data_len = msg.getBytesLen();
    uint8_t *data = msg.getBytes();    
#ifdef DBG_KWP2K
    log_msg("KWP2K::sendMsg(len=%d)\n", data_len);      
#endif      

    uint8_t i;    
    _ser->begin(KWP2K_BAUDRATE);
    for(i=0;i<data_len;i++)
    {        
       // log_msg("%02x ", data[i]);
        _ser->write(data[i]);
    }
    _last_msg_ts = millis();           
    free(data);    
}


// ===============================================
// ================= KWP2KMsg ====================
// ==============================================

KWP2KMsg::KWP2KMsg(void)
{
   this->reset();
}

void KWP2KMsg::reset(void)
{
    memset(&_msg, 0, sizeof(KWP2KMessageStruct));
    _state = IDLE;
    _idx = 0;
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

uint8_t KWP2KMsg::getSId(void)
{
#ifdef DBG_KWP2KMsg
    log_msg("KWP2KMsg::getSId()\n", sid);
#endif    
    return(_msg.SId);
}

uint8_t KWP2KMsg::getCS(void)
{
    return(_msg.CS);
}

uint8_t KWP2KMsg::calcCS(void)
{
    uint8_t data_len = this->getBytesLen();
    uint8_t *buf = (uint8_t*)malloc(data_len);      
    uint8_t i=4;
    uint8_t j;

    buf[0] = _msg.Fmt;
    buf[1] = _msg.Tgt;
    buf[2] = _msg.Src;
    buf[3] = _msg.SId;
    // payload len (>1) means we have more than just a SId
    if (this->getPayloadBytesLen()>1)
    {                
        for (j=1 ; j<this->getPayloadBytesLen() ; j++)
        {
            buf[i++] = _msg.Data[j-1];
        }
    }
    
    // sum up buf contents
    uint8_t sum = 0; 
    for (j=0;j<i;j++)
    {
        sum += buf[j];
    }
    free(buf);    
    return(sum);
}

/*
 * get ptr to message bytes (needs to be free()'d)
 */
uint8_t *KWP2KMsg::getBytes(void)
{
    uint8_t data_len = this->getBytesLen();
    uint8_t *buf = (uint8_t*)malloc(data_len);      
    uint8_t i=4;
    uint8_t j;

    buf[0] = _msg.Fmt;
    buf[1] = _msg.Tgt;
    buf[2] = _msg.Src;
    buf[3] = _msg.SId;
    if (this->getPayloadBytesLen()>1)
    {                
        for (j=1 ; j<this->getPayloadBytesLen() ; j++)
        {
            buf[i++] = _msg.Data[j-1];
        }
    }
    buf[i] = this->calcCS();
    return(buf);
}

/*
 * get payload length without any headers
 */
uint8_t KWP2KMsg::getPayloadBytesLen(void)
{
    return(_msg.Fmt&0x3F);           
}

/*
 * get the payload bytes (SId and following bytes)
 */
uint8_t *KWP2KMsg::getPayloadBytes(void)
{

    uint8_t data_len = this->getPayloadBytesLen();
    uint8_t *buf = (uint8_t*)malloc(data_len);   
    buf[0] = _msg.SId;
    if (data_len > 1)
    {            
            for (uint8_t i=1; i < this->getPayloadBytesLen(); i++)
            {
                buf[i] = _msg.Data[i-1];    
            }            
    }
    return(buf);
}

/*
 * get message length including CRC byte
 */
uint8_t KWP2KMsg::getBytesLen(void)
{
    return(this->getPayloadBytesLen()+3+1);           // 3 = header (fmt,tgt,src) 1 = crc 
}

/*
 * set payload data bytes
 */
void KWP2KMsg::setData(uint8_t idx, uint8_t value)
{
    if (idx<255)
    {
        _msg.Data[idx] = value;
    }
}

void KWP2KMsg::print(void)
{
    log_msg("KWP2KMsg::print()\n");
    log_msg("\t Msg.Fmt: %02x\n", _msg.Fmt);    
    log_msg("\t Msg.Tgt: %02x\n", _msg.Tgt);    
    log_msg("\t Msg.Src: %02x\n", _msg.Src);        
    log_msg("\t Msg.SId: %02x\n", _msg.SId);    
    if (this->getPayloadBytesLen() > 1)
    {
        log_msg("\t Msg.Data:");    
        // skip first payload byte which is always the SId
        for (uint8_t i=1; i < this->getPayloadBytesLen(); i++)
        {
            log_msg("%02x ", _msg.Data[i-1]);    
        }
        log_msg("\n");
    }
    log_msg("\t Msg.CS:  %02x\n", this->calcCS());
    log_msg("\n");
}

void KWP2KMsg::setState(KWP2KMessageState state)
{
    _state = state;
}

void KWP2KMsg::rxByte(uint8_t byte)
{
//#ifdef DBG_KWP2KMsg
    log_msg("KWP2KMsg::rxByte(%d, %02x)\n", _idx, byte);
//#endif
    switch (_idx)
    {
        case 0: _msg.Fmt = byte; break;
        case 1: _msg.Tgt = byte; break;
        case 2: _msg.Src = byte; break;
        case 3: _msg.SId = byte; break;
        default:
                  if (_idx > 3)
                  {
                        uint8_t payload_len = this->getPayloadBytesLen()-1;  // substract SId which we already have
                        if (_idx == 4+payload_len)
                        {                            
                            _msg.CS = byte; 
                        }
                        else                        
                        {
                            _msg.Data[_idx-4] = byte;
                        } 
                  }
    }
    _idx++;    

    if (_idx == this->getBytesLen())
    {       
//#ifdef DBG_KWP2KMsg                    
            log_msg("\t all bytes read !\n");
//#endif       
        if (this->getCS() == this->calcCS())
        {
//#ifdef DBG_KWP2KMsg                    
            log_msg("\t checksum GOOD !\n");
//#endif        
            _state = RESPONSE_RECEIVED; 
        }
        else
        {
            log_msg("received CS = %02x - calculated CS = %02x\n", this->getCS(), this->calcCS());
        }
    }    
    log_msg("debug");
}

KWP2KMessageState KWP2KMsg::getState()
{
    return(_state);
}