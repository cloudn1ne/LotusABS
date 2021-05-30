/*
 EBC430 Functions
 (c) CyberNet <cn@warp.at> 2021
*/
    const KWP_SID_ERR = 0x7F;
    const KWP_SID_STARTCOMMUNICATION = 0x81;
    const KWP_SID_READECUIDENTIFICATION = 0x1A; 
    const KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS = 0x18;
    const KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE = 0x14;
    const KWP_SID_READDATABYLOCALID = 0x21;
    const KWP_SID_INPUTOUTPUTCONTROLBYLOCALID = 0x30;
    
    const EBC_OPT_ALL = 0x80;

    // supported ctrl_param for InputOutputControlByLocalId
    const OP_READ = 0x1;
    const OP_WRITE = 0x7;
    
    // on off flags for supported OP_WRITEs
    const ON = 0x1;
    const OFF = 0x0;

    // supported id for InputOutputControlByLocalId
    const IO_WARNING_LAMP   = 0x1;      // not writeable
    const IO_PUMP_MOTOR     = 0x2;      
    const IO_RELAY          = 0x3;
    const IO_LF_DUMP        = 0x11;
    const IO_LF_ISO         = 0x12;
    const IO_RF_DUMP        = 0x21;
    const IO_RF_ISO         = 0x22;
    const IO_LR_DUMP        = 0x31;
    const IO_LR_ISO         = 0x32;
    const IO_RR_DUMP        = 0x41;
    const IO_RR_ISO         = 0x42;
    const IO_ALL_VALVES     = 0x49;     // control all valves with a bitmask

    function ebc430_init()
    {         
        if (window.location.protocol == "file:")
        {
            // debug mode 
            ws = new WebSocket("ws://10.146.223.10:81/");
        }
        else
        {
            ws = new WebSocket(((window.location.protocol === "https:") ? "wss://" : "ws://") + window.location.host + ":81/");
        }        
        return(ws);
    }

    function resp_ErrorMessage(data)
    {        
        if (data.errcode == 0xFF)
        {
            // timeout
            $('#StartCommunicationModal').modal('hide');
            $('#TimeoutModal').modal('show');
        }
       // console.log("resp_ErrorMessage()");
       // console.log(data);                    
        $('#errormessage_text').html(KWPErrorToText(parseInt(data.errcode)));
        $("#errormessage").fadeTo(3000, 500).slideUp(500, function() {
            $("#errormessage").slideUp(500);            
        });        

    }


    function req_StartCommunication()
    {              
      //  console.log("req_StartCommunication()")                     
        var msg = {
            sid: KWP_SID_STARTCOMMUNICATION                            
        }            
        ws.send(JSON.stringify(msg));                           
        $("#StartCommunicationModal").modal('show');          
        $('#StartCommunicationModalTitle').html("Connecting to ECU");  
    }

    function req_ReadECUIdentification()
    {
       // console.log("req_ReadECUIdentification()");
        var msg = {
            sid: KWP_SID_READECUIDENTIFICATION,
            option: EBC_OPT_ALL                
        }
        ws.send(JSON.stringify(msg));
    }

    function req_ReadDiagnosticTroubleCodeByStatus()
    {
       // console.log("req_ReadDiagnosticTroubleCodeByStatus()");
        var msg = {
            sid: KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS                       
        }
        ws.send(JSON.stringify(msg));
    }

    function req_ClearDiagnosticInformationService()
    {
       // console.log("req_ClearDiagnosticInformationService()");
        var msg = {
            sid: KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE                       
        }
        ws.send(JSON.stringify(msg));
    }

    function req_ReadDataByLocalId(id, dtc=0)
    {
       // console.log("req_ReadDataByLocalId()");

        if (id == 0x10)             // read data for DTC code given
        {
            if (dtc == 0x0) 
            {
                alert("No DTC code provided to req_ReadDataByLocalId() !");
                return;
            }
            var msg = {
                sid: KWP_SID_READDATABYLOCALID,
                record: id,
                dtc: dtc
            }
        }
        else
        {
            var msg = {
                sid: KWP_SID_READDATABYLOCALID,
                record: id                
            }
        }
        ws.send(JSON.stringify(msg));
    }

    function req_InputOutputControlByLocalId(id, ctrl_param, ctrl_state=0x0)
    {
       // console.log("req_InputOutputControlByLocalId()");
                        
        if (ctrl_param == 0x7)
        {
            var msg = {
                sid: KWP_SID_INPUTOUTPUTCONTROLBYLOCALID,
                id: id,
                ctrl_param: ctrl_param,
                ctrl_state: ctrl_state
            }            
        }                
        else
        {
            var msg = {
                sid: KWP_SID_INPUTOUTPUTCONTROLBYLOCALID,
                id: id,
                ctrl_param: ctrl_param                
            }
            
        }
        ws.send(JSON.stringify(msg));
    }
  
    // convert DTC Code to Text
    function DTCtoString(dtc_code)
    {
        switch (dtc_code)
        {
            case 0x0035: return("Front left wheel speed sensor:Short circuit or circuit open");
            case 0x0040: return("Front right wheel speed sensor:Short circuit or circuit open");
            case 0x0045: return("Rear left wheel speed sensor:Short circuit or circuit open");
            case 0x0050: return("Rear right wheel speed sensor:Short circuit or circuit open")
            case 0x0060: return("Front left outlet solenoid valve circuit malfunction");
            case 0x0065: return("Front left inlet solenoid valve circuit malfunction");
            case 0x0070: return("Front right outlet solenoid valve circuit malfunction");
            case 0x0075: return("Front right inlet solenoid valve circuit malfunction");
            case 0x0080: return("Rear left outlet solenoid valve circuit malfunction");
            case 0x0085: return("Rear left inlet solenoid valve circuit malfunction");
            case 0x0090: return("Rear right outlet solenoid valve circuit malfunction");
            case 0x0095: return("Rear right inlet solenoid valve circuit malfunction");
            case 0x0110: return("Return pump: circuit open or shorted, locked or shorted");
            case 0x0121: return("Valve relay circuit malfunction");
            case 0x0161: return("Brake light switch fault");
            case 0x0232: return("Brake system telltale voltage: high,low or open circuit");
            case 0x0245: return("Wheel speed: sensor erratic signal or error");
            case 0x0252: return("Speed calculation error between 8 bits");
            case 0x0550: return("Replace electronic control unit");
            case 0x0556: return("Watchdog error");
            case 0x0560: return("Replace electronic control unit");
            case 0x0561: return("8 bit RAM / ROM error");
            case 0x0563: return("16 bit ROM error");
            case 0x0564: return("16 bit RAM error");            
            case 0x0800: return("Switched battery voltage: high (valve relay) or low (valve relay)");
            default: return("Unknown DTC");
        }
    }

    function DTCStatusToCause(dtc_status)
    {
        dtc_cause = dtc_status & 0xF;
        switch (dtc_cause)
        {
            case 0x0: return("No additional information available for this DTC");
            case 0x1: return("Over the maximum threshold");
            case 0x2: return("Below minimum threshold");
            case 0x4: return("No signal");
            case 0x8: return("Invalid signal");
        }
    }

    function DTCStatusToMIL(dtc_status)
    {
        return ( (dtc_status>>7) & 0x1 );        
    }

    function DTCStatusToTestComplete(dtc_status)
    {
        return( ! (dtc_status>>4) & 0x1 );       
    }
    
    function DTCStatusToStorageStatus(dtc_status)
    {
        return( ! (dtc_status>>6) & 0x3 );       
    }

    function KWPErrorToText(errcode)
    {    
        switch (errcode)
        {
            case 0x10: return("<b>General reject</b> Service is refused but the ECU does not specify the reason.");
            case 0x11: return("<b>Service Not supported</b> The request will not be taken into consideration because the Service is not available.");
            case 0x12: return("<b>Sub Function Not Supported – Invalid Format</b> The request will not be considered because the service is not available or the format of the argument bytes does not respect the format prescribed for the specified Service.");
            case 0x22: return("<b>Condition Not Correct or Request Sequence Error</b> The requested action will not be taken because the ECU prerequisite are not met.");
            case 0x31: return("<b>Request Out Of Range</b> The requested action will not be taken because the ECU detects the requested message contains a data byte which attempts to substitute a value beyond its range authority.");
            default: return("Unknown KWP2000 error 0x"+errcode.toString(16));
        }
    }

    function ValveBitToName(valve_bit)
    {
        let name='';
        switch (valve_bit)   // 7-0
        {
                case 7: name = "lf-dump";    break;
                case 6: name = "lf-iso";   break;              
                case 5: name = "rf-dump";    break;
                case 4: name = "rf-iso";   break; 
                case 3: name = "lr-dump";    break;
                case 2: name = "lr-iso";   break;
                case 1: name = "rr-dump";    break;
                case 0: name = "rr-iso";   break;   
        }         
        return(name)
    }

    function ValveIDToBit(valve_id)
    {
        switch(valve_id)
        {
                case IO_LF_DUMP:      return(7);
                case IO_LF_ISO:       return(6);
                case IO_RF_DUMP:      return(5);
                case IO_RF_ISO:       return(4);
                case IO_LR_DUMP:      return(3);
                case IO_LR_ISO:       return(2);
                case IO_RR_DUMP:      return(1);
                case IO_RR_ISO:       return(0);
        }
    }