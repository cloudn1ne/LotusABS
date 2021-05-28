
    const KWP_SID_ERR = 0x7F;
    const KWP_SID_STARTCOMMUNICATION = 0x81;
    const KWP_SID_READECUIDENTIFICATION = 0x1A; 
    const KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS = 0x18;
    const KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE = 0x14;
    
    const EBC_OPT_ALL = 0x80;



    function ebc430_init()
    {
           // ws = new WebSocket(((window.location.protocol === "https:") ? "wss://" : "ws://") + window.location.host + ":81/");
           ws = new WebSocket("ws://10.146.223.10:81/");
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
        console.log("resp_ErrorMessage()");
        console.log(data);            
    }


    function req_StartCommunication()
    {              
        console.log("req_StartCommunication()")                     
        var msg = {
            sid: KWP_SID_STARTCOMMUNICATION                            
        }            
        ws.send(JSON.stringify(msg));                           
        $("#StartCommunicationModal").modal('show');          
        $('#StartCommunicationModalTitle').html("Connecting to ECU");  
    }

    function req_ReadECUIdentification()
    {
        console.log("req_ReadECUIdentification()");
        var msg = {
            sid: KWP_SID_READECUIDENTIFICATION,
            option: EBC_OPT_ALL                
        }
        ws.send(JSON.stringify(msg));
    }

    function req_ReadDiagnosticTroubleCodeByStatus()
    {
        console.log("req_ReadDiagnosticTroubleCodeByStatus()");
        var msg = {
            sid: KWP_SID_READDIAGNOSTICTROUBLECODESBYSTATUS                       
        }
        ws.send(JSON.stringify(msg));
    }

    function req_ClearDiagnosticInformationService()
    {
        console.log("req_ClearDiagnosticInformationService()");
        var msg = {
            sid: KWP_SID_CLEARDIAGNOSTICINFORMATIONSERVICE                       
        }
        ws.send(JSON.stringify(msg));
    }

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
            case 0x0232: return("Brake system telltale voltage: high,low or open circui");
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

