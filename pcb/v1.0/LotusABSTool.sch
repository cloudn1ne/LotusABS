EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L WEMOS-D1-MINI:WeMos_D1_mini U1
U 1 1 60AA261D
P 5600 2100
F 0 "U1" H 5600 2843 60  0000 C CNN
F 1 "WeMos_D1_mini" H 5600 2737 60  0000 C CNN
F 2 "WEMOS-D1-MINI:wemos-d1-mini-with-pin-header" H 6150 1400 60  0001 C CNN
F 3 "" H 5600 2631 60  0000 C CNN
	1    5600 2100
	1    0    0    -1  
$EndComp
$Comp
L STMicroelectronics:L9637D IC1
U 1 1 60AA6DD0
P 2400 2100
F 0 "IC1" H 2400 2765 50  0000 C CNN
F 1 "L9637D" H 2400 2674 50  0000 C CNN
F 2 "Package_SO:SO-8_3.9x4.9mm_P1.27mm" H 2400 2250 50  0001 C CNN
F 3 "" H 2400 2100 50  0001 C CNN
	1    2400 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:R 510R1
U 1 1 60AABA3C
P 1650 1700
F 0 "510R1" V 1443 1700 50  0000 C CNN
F 1 "R" V 1534 1700 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1580 1700 50  0001 C CNN
F 3 "~" H 1650 1700 50  0001 C CNN
	1    1650 1700
	0    1    1    0   
$EndComp
Wire Wire Line
	2000 1700 1900 1700
Text GLabel 1250 1900 0    50   Input ~ 0
K-LINE
Wire Wire Line
	2000 1900 1450 1900
Wire Wire Line
	1500 1700 1450 1700
Wire Wire Line
	1450 1700 1450 1900
Connection ~ 1450 1900
Wire Wire Line
	1450 1900 1250 1900
NoConn ~ 2000 2200
$Comp
L power:GND #PWR03
U 1 1 60AAD379
P 2900 2500
F 0 "#PWR03" H 2900 2250 50  0001 C CNN
F 1 "GND" H 2905 2327 50  0000 C CNN
F 2 "" H 2900 2500 50  0001 C CNN
F 3 "" H 2900 2500 50  0001 C CNN
	1    2900 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2400 2900 2400
Wire Wire Line
	2900 2400 2900 2500
$Comp
L power:+5V #PWR02
U 1 1 60AADEA7
P 2900 1500
F 0 "#PWR02" H 2900 1350 50  0001 C CNN
F 1 "+5V" H 2915 1673 50  0000 C CNN
F 2 "" H 2900 1500 50  0001 C CNN
F 3 "" H 2900 1500 50  0001 C CNN
	1    2900 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 1700 2900 1700
Wire Wire Line
	2900 1700 2900 1500
Text GLabel 3250 2000 2    50   Input ~ 0
ESP-RX
Text GLabel 6350 1750 2    50   Input ~ 0
ESP-TX
Wire Wire Line
	2800 2000 3250 2000
Wire Wire Line
	3250 1900 2800 1900
NoConn ~ 2800 2200
$Comp
L Device:C 100n1
U 1 1 60AB53AE
P 1450 2250
F 0 "100n1" H 1565 2296 50  0000 L CNN
F 1 "C" H 1565 2205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1488 2100 50  0001 C CNN
F 3 "~" H 1450 2250 50  0001 C CNN
	1    1450 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 1900 1450 2100
$Comp
L power:GND #PWR01
U 1 1 60AB6062
P 1450 2550
F 0 "#PWR01" H 1450 2300 50  0001 C CNN
F 1 "GND" H 1455 2377 50  0000 C CNN
F 2 "" H 1450 2550 50  0001 C CNN
F 3 "" H 1450 2550 50  0001 C CNN
	1    1450 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 2400 1450 2550
$Comp
L power:+5V #PWR06
U 1 1 60AB66BC
P 6650 2450
F 0 "#PWR06" H 6650 2300 50  0001 C CNN
F 1 "+5V" H 6665 2623 50  0000 C CNN
F 2 "" H 6650 2450 50  0001 C CNN
F 3 "" H 6650 2450 50  0001 C CNN
	1    6650 2450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2450 6650 2450
NoConn ~ 5100 2450
$Comp
L power:GND #PWR05
U 1 1 60AB79AB
P 6200 2550
F 0 "#PWR05" H 6200 2300 50  0001 C CNN
F 1 "GND" H 6205 2377 50  0000 C CNN
F 2 "" H 6200 2550 50  0001 C CNN
F 3 "" H 6200 2550 50  0001 C CNN
	1    6200 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 2350 6200 2350
Wire Wire Line
	6200 2350 6200 2550
$Comp
L Regulator_Linear:BA05FP-E2 U2
U 1 1 60ABD5D7
P 5550 4500
F 0 "U2" H 5550 4742 50  0000 C CNN
F 1 "BA05FP-E2" H 5550 4651 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TO-252-2" H 5550 4700 50  0001 C CNN
F 3 "http://www.diodes.com/datasheets/AP1117.pdf" H 5650 4250 50  0001 C CNN
	1    5550 4500
	1    0    0    -1  
$EndComp
Text Notes 1800 1000 0    197  ~ 39
K-LINE
Text Notes 6400 1250 0    197  ~ 39
ESP\n
Text Notes 5250 3800 0    197  ~ 39
POWER
$Comp
L power:+12V #PWR07
U 1 1 60AC0562
P 4450 4400
F 0 "#PWR07" H 4450 4250 50  0001 C CNN
F 1 "+12V" H 4465 4573 50  0000 C CNN
F 2 "" H 4450 4400 50  0001 C CNN
F 3 "" H 4450 4400 50  0001 C CNN
	1    4450 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 4500 5100 4500
$Comp
L power:GND #PWR08
U 1 1 60AC0EAE
P 5550 5150
F 0 "#PWR08" H 5550 4900 50  0001 C CNN
F 1 "GND" H 5555 4977 50  0000 C CNN
F 2 "" H 5550 5150 50  0001 C CNN
F 3 "" H 5550 5150 50  0001 C CNN
	1    5550 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 4800 5550 4950
$Comp
L Device:C C2
U 1 1 60AC1B93
P 6000 4750
F 0 "C2" H 6115 4796 50  0000 L CNN
F 1 "22uF" H 6115 4705 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_10x10.5" H 6038 4600 50  0001 C CNN
F 3 "~" H 6000 4750 50  0001 C CNN
	1    6000 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 4500 6000 4500
Wire Wire Line
	6000 4500 6000 4600
Wire Wire Line
	6000 4900 6000 4950
Wire Wire Line
	6000 4950 5550 4950
Connection ~ 5550 4950
$Comp
L Device:C C1
U 1 1 60AC2A01
P 5100 4750
F 0 "C1" H 5215 4796 50  0000 L CNN
F 1 "330nF" H 5215 4705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5138 4600 50  0001 C CNN
F 3 "~" H 5100 4750 50  0001 C CNN
	1    5100 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 4600 5100 4500
Wire Wire Line
	5550 4950 5100 4950
Wire Wire Line
	5100 4950 5100 4900
$Comp
L power:+5V #PWR09
U 1 1 60AC3892
P 6700 4400
F 0 "#PWR09" H 6700 4250 50  0001 C CNN
F 1 "+5V" H 6715 4573 50  0000 C CNN
F 2 "" H 6700 4400 50  0001 C CNN
F 3 "" H 6700 4400 50  0001 C CNN
	1    6700 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 4500 6700 4500
Wire Wire Line
	6700 4500 6700 4400
Connection ~ 6000 4500
$Comp
L Device:R 510R3
U 1 1 60AC4540
P 6700 4750
F 0 "510R3" H 6630 4704 50  0000 R CNN
F 1 "R" H 6630 4795 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 6630 4750 50  0001 C CNN
F 3 "~" H 6700 4750 50  0001 C CNN
	1    6700 4750
	-1   0    0    1   
$EndComp
Wire Wire Line
	6700 4500 6700 4600
Connection ~ 6700 4500
$Comp
L Device:LED D3
U 1 1 60AC60B1
P 6700 5150
F 0 "D3" V 6739 5032 50  0000 R CNN
F 1 "LED_PWR" V 6648 5032 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6700 5150 50  0001 C CNN
F 3 "~" H 6700 5150 50  0001 C CNN
	1    6700 5150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6700 4900 6700 5000
Wire Wire Line
	5550 4950 5550 5150
$Comp
L power:GND #PWR010
U 1 1 60AC78BF
P 6700 5450
F 0 "#PWR010" H 6700 5200 50  0001 C CNN
F 1 "GND" H 6705 5277 50  0000 C CNN
F 2 "" H 6700 5450 50  0001 C CNN
F 3 "" H 6700 5450 50  0001 C CNN
	1    6700 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 5300 6700 5450
$Comp
L Device:D D2
U 1 1 60ACE31B
P 4800 4500
F 0 "D2" H 4800 4283 50  0000 C CNN
F 1 "D" H 4800 4374 50  0000 C CNN
F 2 "Diode_SMD:D_SMA-SMB_Universal_Handsoldering" H 4800 4500 50  0001 C CNN
F 3 "~" H 4800 4500 50  0001 C CNN
	1    4800 4500
	-1   0    0    1   
$EndComp
Wire Wire Line
	5100 4500 4950 4500
Connection ~ 5100 4500
Wire Wire Line
	4650 4500 4450 4500
Wire Wire Line
	4450 4500 4450 4400
Text GLabel 6350 1850 2    50   Input ~ 0
ESP-RX
Wire Wire Line
	6350 1850 6100 1850
Wire Wire Line
	6350 1750 6100 1750
$Comp
L Device:R 510R2
U 1 1 60AD21D9
P 4550 2350
F 0 "510R2" H 4480 2304 50  0000 R CNN
F 1 "R" H 4480 2395 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4480 2350 50  0001 C CNN
F 3 "~" H 4550 2350 50  0001 C CNN
	1    4550 2350
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D1
U 1 1 60AD21E4
P 4550 2750
F 0 "D1" V 4589 2632 50  0000 R CNN
F 1 "LED_OK" V 4498 2632 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4550 2750 50  0001 C CNN
F 3 "~" H 4550 2750 50  0001 C CNN
	1    4550 2750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4550 2500 4550 2600
$Comp
L power:GND #PWR04
U 1 1 60AD21EF
P 4550 3050
F 0 "#PWR04" H 4550 2800 50  0001 C CNN
F 1 "GND" H 4555 2877 50  0000 C CNN
F 2 "" H 4550 3050 50  0001 C CNN
F 3 "" H 4550 3050 50  0001 C CNN
	1    4550 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 2900 4550 3050
Wire Wire Line
	5100 1950 4550 1950
Wire Wire Line
	4550 1950 4550 2200
NoConn ~ 5100 2350
NoConn ~ 5100 2250
NoConn ~ 5100 2150
NoConn ~ 5100 2050
NoConn ~ 5100 1850
NoConn ~ 5100 1750
$Comp
L Connector:DB9_Male J1
U 1 1 60ADDAD3
P 2100 5550
F 0 "J1" V 1972 6105 50  0000 L CNN
F 1 "DB9_Male" V 2063 6105 50  0000 L CNN
F 2 "Connector_Dsub:DSUB-9_Male_Vertical_P2.77x2.84mm_MountingHoles" H 2100 5550 50  0001 C CNN
F 3 " ~" H 2100 5550 50  0001 C CNN
	1    2100 5550
	0    1    1    0   
$EndComp
Text Notes 1400 3750 0    197  ~ 39
CABLE
$Comp
L power:+12V #PWR0101
U 1 1 60ADF921
P 2400 4550
F 0 "#PWR0101" H 2400 4400 50  0001 C CNN
F 1 "+12V" H 2415 4723 50  0000 C CNN
F 2 "" H 2400 4550 50  0001 C CNN
F 3 "" H 2400 4550 50  0001 C CNN
	1    2400 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 5250 2400 4550
$Comp
L power:GND #PWR0102
U 1 1 60AE1345
P 1800 4500
F 0 "#PWR0102" H 1800 4250 50  0001 C CNN
F 1 "GND" H 1805 4327 50  0000 C CNN
F 2 "" H 1800 4500 50  0001 C CNN
F 3 "" H 1800 4500 50  0001 C CNN
	1    1800 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 5250 1900 4450
Wire Wire Line
	1900 4450 1800 4450
Wire Wire Line
	1800 4450 1800 4500
Text GLabel 2300 4900 1    50   Input ~ 0
K-LINE
Wire Wire Line
	2300 4900 2300 5250
Text Notes 1500 5850 0    50   ~ 0
Note: DB9 Pin 4 - to OBD Pin 12 (ABS K-LINE)
Text GLabel 2500 4900 1    50   Input ~ 0
CAN-L
Wire Wire Line
	2500 4900 2500 5250
Text GLabel 2100 4900 1    50   Input ~ 0
CAN-H
Wire Wire Line
	2100 5250 2100 4900
Text GLabel 3250 1900 2    50   Input ~ 0
ESP-TX
$Comp
L power:+12V #PWR0103
U 1 1 60AF405A
P 1900 1500
F 0 "#PWR0103" H 1900 1350 50  0001 C CNN
F 1 "+12V" H 1915 1673 50  0000 C CNN
F 2 "" H 1900 1500 50  0001 C CNN
F 3 "" H 1900 1500 50  0001 C CNN
	1    1900 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 1500 1900 1700
Connection ~ 1900 1700
Wire Wire Line
	1900 1700 1800 1700
$EndSCHEMATC