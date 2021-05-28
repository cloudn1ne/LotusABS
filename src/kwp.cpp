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
    _is_initialized = false;
    _is_communication_started = false;
    _last_tx_msg_ts = 0;           // never    
    _last_rx_msg_ts = 0;           // never

    // reset tx queue to be empty
    for (uint8_t i=0;i<KWP2K_TX_QUEUE_SIZE;i++)
    {
        _tx_queue[i]=NULL;
    }
    _waiting_for_reply = false;    
}

void KWP2K::setTimeoutCallback(void (*callback)(void))
{
    cb_Timeout = callback;
}

void KWP2K::setCallback(uint8_t SId, void (*callback)(uint8_t *payload_bytes, uint8_t payload_len))
{
    switch (SId)
    {        
        case KWP_SID_ERR:                                   cb_ErrorMessage                         = callback; break;
        case KWP_SID_STARTCOMMUNICATION:                    cb_StartCommunication                   = callback; break;
        case KWP_SID_READECUIDENTIFICATION:                 cb_ReadECUIdentification                = callback; break;
        case KWP_SID_READDATABYLOCALID:                     cb_ReadDataByLocalId                    = callback; break;
        case KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS:    cb_ReadDiagnosticTroubleCodesByStatus   = callback; break;
        case KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE:     cb_ClearDiagnosticInformationService    = callback; break;
        case KWP_SID_INPUTOUTPUTCONTROLBYLOCALID:           cb_InputOutputControlByLocalId          = callback; break;
    }
}

/*
* add KWP2KMsg to TX queue
*/
bool KWP2K::pushTXQueue(KWP2KMsg *msg)
{
#ifdef DBG_KWP2K
        log_msg("KWP2K::addToTXQueue(SId=%02x)\n", msg->getSId());      
#endif      
    uint8_t i;

    for (i=0;i<KWP2K_TX_QUEUE_SIZE;i++)
    if (_tx_queue[i] == NULL)
    {
        _tx_queue[i] = msg;
#ifdef DBG_KWP2K
        log_msg("\t added to slot #%d\n", i);      
#endif     
        return(true);
    }       
#ifdef DBG_KWP2K
        log_msg("\t ERROR no emtpy queue slot found\n");  
#endif  
    return(false);   
}

/*
* pop KWP2KMsg from TX queue
* return: NULL or a KWP2K message if one is pending to be sent
*/
KWP2KMsg *KWP2K::popTXQueue(void)
{  
    uint8_t i;
    KWP2KMsg *retptr;

    for (i=0;i<KWP2K_TX_QUEUE_SIZE;i++)
    if (_tx_queue[i] != NULL)
    {
        retptr = _tx_queue[i];
        _tx_queue[i] = NULL;     // mark empty        
#ifdef DBG_KWP2K
        log_msg("KWP2K::popTXQueue()\n"); 
        log_msg("\t returned KWP2KMsg (SId=%02x) from slot #%d\n", retptr->getSId(), i);      
#endif     
        return(retptr);
    }       
    return(NULL);   
}

/*
* peek KWP2KMsg in TX queue
* return: true if some message wants to be send, otherwise false
*/
KWP2KMsg *KWP2K::peekTXQueue(void)
{  
    uint8_t i;
    
    for (i=0;i<KWP2K_TX_QUEUE_SIZE;i++)
    if (_tx_queue[i] != NULL)
    {
      return(_tx_queue[i]);
    }       
    return(NULL);   
}

//////////////////////////////////////////////
// Process KWP2K Handler
//////////////////////////////////////////////
void KWP2K::process()
{      
    KWP2KMsg *msg;
    
    // should we trigger a fastInit ?
    if (_is_initialized && (_last_tx_msg_ts+KWP2K_SESSION_TIMEOUT < millis()))
    {
#ifdef DBG_KWP2K
        log_msg("KWP2K::process() no longer initialized\n");      
#endif  
        _is_initialized = false;           // Communication Session no longer initialized
        _is_communication_started = false; // Communication Session timed out        
    }            

    // we are not waiting for a reply to a previous message and there is a new one to send
    // if (!_waiting_for_reply && (msg=this->popTXQueue()))    


    if ( (_last_rx_msg_ts+KWP2K_INTERMSG_DELAY < millis() ) &&      // wait for intermessage delay (between last rx and next tx)
         !_waiting_for_reply &&                                     // we dont have a reply that we expect pending
         this->peekTXQueue()                                        // there is a new message waiting to be sent
       )    // message to be sent pending ?
    {    
        // are we initalized ? 
        if (!_is_initialized)
        {
            this->fastInit();
        }                
        // do we have a comm session
        if (!_is_communication_started && (this->peekTXQueue()->getSId()!=KWP_SID_STARTCOMMUNICATION))
        {                        
            msg = this->getStartCommunicationMsg();       // send the StartCommunication message
        }
        else
        {
            msg = this->popTXQueue();                   // send the pending message
        }
        
        this->sendMsg(msg);                                
        delete msg;

        _waiting_for_reply_timeout = millis();
        _waiting_for_reply = true;

        _msgrx.reset();         // make ready the _msgrx buffer                
    }

    // are we waiting for a reply ?
    if (_waiting_for_reply)
    {               
        // have we been waiting too long ?
        if (_waiting_for_reply_timeout + KWP2K_REPLY_TIMEOUT < millis())
        {
            _waiting_for_reply = false; 
            _is_communication_started = false;
            _is_initialized = false;
            this->cb_Timeout(); 
            log_msg("WARNING: expected reply timed out\n");
        }
        else
        {
            // attempt to receive reply from ECU
            _msgrx.setState(IDLE);
            while (_ser->available())
            {                
                uint8_t in = _ser->read();      
                _msgrx.rxByte(in);
                _last_rx_msg_ts = millis();
            }       
        }                        
    }    

    // CS was ok, so the response has been received
    if (_msgrx.getState() == RESPONSE_RECEIVED) 
    {
        
    #ifdef DBG_KWP2K        
        log_msg(">> RESPONSE RECEIVED for SId=%02x len=%d\n", (_msgrx.getSId()-0x40), _msgrx.getPayloadBytesLen());        
    #endif
        /*
        uint8_t i;
        uint8_t *payload = _msgrx.getPayloadBytes();        
        for (i=0;i<_msgrx.getPayloadBytesLen();i++)        
        {
            log_msg("%02x ", payload[i]);

        }
        free(payload);
        */
        switch(_msgrx.getSId())
        {
            case KWP_SID_ERR:                                       this->cb_ErrorMessage(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break; 
            case KWP_SID_STARTCOMMUNICATION_POS:                    _is_communication_started=true; this->cb_StartCommunication(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
            case KWP_SID_READECUIDENTIFICATION_POS:                 this->cb_ReadECUIdentification(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
            case KWP_SID_READDATABYLOCALID_POS:                     this->cb_ReadDataByLocalId(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
            case KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS_POS:    this->cb_ReadDiagnosticTroubleCodesByStatus(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
            case KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE_POS:     this->cb_ClearDiagnosticInformationService(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
            case KWP_SID_INPUTOUTPUTCONTROLBYLOCALID_POS:           this->cb_InputOutputControlByLocalId(_msgrx.getPayloadBytes(), _msgrx.getPayloadBytesLen()); break;
        }                
        _waiting_for_reply = false;
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
    _is_initialized = true;
}

// Functions

/*
* Get StartCommunication message (used for auto connect)
*/
KWP2KMsg *KWP2K::getStartCommunicationMsg()
{        
    KWP2KMsg *msg;
    msg = new KWP2KMsg();
    msg->setFmt(KWPMSG_HM2, 1);  
    msg->setSrc(_Testeraddress);  
    msg->setTgt(_ECUaddress);
    msg->setSId(KWP_SID_STARTCOMMUNICATION);  
    msg->setState(QUEUED);
    return(msg);
}

/*
* Send StartCommunication message
*/
void KWP2K::StartCommunication(void)
{        
//    if (!_is_communication_started)
    //{
        KWP2KMsg *msg;
        msg = new KWP2KMsg();
        msg->setFmt(KWPMSG_HM2, 1);  
        msg->setSrc(_Testeraddress);  
        msg->setTgt(_ECUaddress);
        msg->setSId(KWP_SID_STARTCOMMUNICATION);      
        msg->setState(QUEUED);
    // msg->print();     
        this->pushTXQueue(msg);
   // }
}


void KWP2K::ReadECUIdentification(uint8_t option)
{        
    KWP2KMsg *msg;
    msg = new KWP2KMsg();
    msg->setFmt(KWPMSG_HM2, 2);  
    msg->setSrc(_Testeraddress);  
    msg->setTgt(_ECUaddress);
    msg->setSId(KWP_SID_READECUIDENTIFICATION);  
    msg->setData(0, option);
    msg->setState(QUEUED);
   // msg->print();     
    this->pushTXQueue(msg);
}

void KWP2K::ReadDataByLocalId(uint8_t record, uint16_t dtc)
{        
    KWP2KMsg *msg;
    msg = new KWP2KMsg();
    if (record == 0x10)  
        msg->setFmt(KWPMSG_HM2, 4);  
    else
        msg->setFmt(KWPMSG_HM2, 2);      
    msg->setSrc(_Testeraddress);  
    msg->setTgt(_ECUaddress);
    msg->setSId(KWP_SID_READDATABYLOCALID); 
    msg->setData(0, record);  
    if (record == 0x10)                 // Local Identifier == 0x10 ?
    {
        msg->setData(1, dtc>>8);        // DTC Highbyte
        msg->setData(2, dtc&0xFF);      // DTC Lowbyte
    }
    msg->setState(QUEUED);
    // msg->print();     
    this->pushTXQueue(msg);
}

void KWP2K::ReadDiagnosticTroubleCodesByStatus(void)
{
    KWP2KMsg *msg;
    msg = new KWP2KMsg();
    msg->setFmt(KWPMSG_HM2, 4);  
    msg->setSrc(_Testeraddress);  
    msg->setTgt(_ECUaddress);
    msg->setSId(KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS); 
    msg->setData(0, 0x0);     // status OfDTC (independent of their status)
    msg->setData(1, 0xFF);    // Group (0xFF00) Highbyte
    msg->setData(2, 0x0);     // Group (0xFF00) Lowbyte
    msg->setState(QUEUED);
   // msg->print();     
    this->pushTXQueue(msg);
}

void KWP2K::ClearDiagnosticInformationService(void)
{
    KWP2KMsg *msg;
    msg = new KWP2KMsg();
    msg->setFmt(KWPMSG_HM2, 3);  
    msg->setSrc(_Testeraddress);  
    msg->setTgt(_ECUaddress);
    msg->setSId(KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE);     
    msg->setData(0, 0xFF);    // Group (0xFF00) Highbyte
    msg->setData(1, 0x0);     // Group (0xFF00) Lowbyte
    msg->setState(QUEUED);
   // msg->print();     
    this->pushTXQueue(msg);
}

void KWP2K::InputOutputControlByLocalId(uint8_t id, uint8_t ctrl_param, uint8_t ctrl_state)
{
    KWP2KMsg *msg;
    msg = new KWP2KMsg();
    if (ctrl_param == 0x7)
        msg->setFmt(KWPMSG_HM2, 4);  
    else
        msg->setFmt(KWPMSG_HM2, 3);  

    msg->setSrc(_Testeraddress);  
    msg->setTgt(_ECUaddress);
    msg->setSId(KWP_SID_INPUTOUTPUTCONTROLBYLOCALID);     
    msg->setData(0, id);       
    msg->setData(1, ctrl_param);     
    if (ctrl_param == 0x7)
    {
        msg->setData(2, ctrl_state);     
    }
    msg->setState(QUEUED);
    // msg->print();     
    this->pushTXQueue(msg);
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
void KWP2K::sendMsg(KWP2KMsg *msg)
{   
    uint8_t i;     
    uint8_t data_len = msg->getBytesLen();
    uint8_t *data = msg->getBytes();    
#ifdef DBG_KWP2K
    log_msg("KWP2K::sendMsg(SId=%02x len=%d)\n", msg->getSId(), data_len);      
#endif      
       
    _ser->begin(KWP2K_BAUDRATE);
    for(i=0;i<data_len;i++)
    {        
       // log_msg("%02x ", data[i]);
        _ser->write(data[i]);
    }
    _last_tx_msg_ts = millis();           

    // the that we send is mirrored on the Kline so we receive it back immediatly            
    for (uint8_t i=0;i<data_len;i++)
    {
        if (_ser->read() != data[i])
        {
            log_msg("KWP2K::process() error reading back sent message\n");                
            break;
        }
    }                  
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
  //  memset(&_msg, 0, sizeof(KWP2KMessageStruct));
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
#ifdef DBG_KWP2KMsg
    log_msg("KWP2KMsg::rxByte(%d, %02x)\n", _idx, byte);
#endif
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
#ifdef DBG_KWP2KMsg                    
            log_msg("\t all bytes read !\n");
#endif       
        if (this->getCS() == this->calcCS())
        {
#ifdef DBG_KWP2KMsg                    
            log_msg("\t checksum GOOD !\n");
#endif        
            _state = RESPONSE_RECEIVED; 
        }
        else
        {
            log_msg("ERROR: received CS = %02x - calculated CS = %02x\n", this->getCS(), this->calcCS());
        }
    }        
}

KWP2KMessageState KWP2KMsg::getState()
{
    return(_state);
}