# LotusABS

<img src='https://raw.githubusercontent.com/cloudn1ne/LotusABS/master/pic/board1.jpg' width='300'>
<img src='https://raw.githubusercontent.com/cloudn1ne/LotusABS/master/pic/board2.jpg' width='300'>
<img src='https://raw.githubusercontent.com/cloudn1ne/LotusABS/master/pic/index.png' width='300'>
<img src='https://raw.githubusercontent.com/cloudn1ne/LotusABS/master/pic/dtc.png' width='300'>
<img src='https://raw.githubusercontent.com/cloudn1ne/LotusABS/master/pic/valves.png' width='300'>


## What can you do with it ?

The software part is a pretty minimum implementation of the KWP2000 via K-LINE as used by the EBC430 EBC.
The supported Service IDs are:

Function | Usage
-------- | -----
StartCommunication | start a KWP2000 session
ReadECUIdentification | ECU details, nothing new here - same as listed in the Lotus Service Manual
ReadDataByLocalId | battery voltage, wheel speeds (not implemented, because my car is not moving atm)
ReadDiagnosticTroubleCodeByStatus | read DTC codes and status reasons
ClearDiagnosticInformationService | clear and DTCs
InputOutputControlByLocalId | control the valves, pump, relay

So with that you can establish communication with the ECU, get software/hardware versions, battery voltage (wheels speeds later on), read and clear DTCs, and most importantly
control the valves, pump and valve relay to aid with bleeding.

## How can i use it ?

You need to solder up the little ESP8266 based adapter (or put it on breadboard if you are adventurous. A proven PCB design and circuit is included in the ./pcb folder.
It was made with KiCAD (www.kicad.org)

## Whats the difference to existing KLINE/OBD Tools ?

Not much - i didnt have one, so i had to roll my own - the most notible difference is that the KLINE line has to be on PIN #12 of the OBD connector (usualy it would be Pin #7)
And you wont have a GUI to control stuff.

## Software
The project is based on PLATFORM.IO - download VS Code and install platform IO (https://platformio.org/) 

## Hardware
see KiCAD


### As usual that tool comes with no warranty, it might blow up your airbags (same KLINE) or set your car on fire. Good luck ;-)
