EESchema Schematic File Version 4
LIBS:rmpd_player-cache
EELAYER 26 0
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
L ESP32-footprints-Shem-Lib:ESP32-WROOM U?
U 1 1 5C001F56
P 6000 2200
F 0 "U?" H 5975 3587 60  0000 C CNN
F 1 "ESP32-WROOM" H 5975 3481 60  0000 C CNN
F 2 "ESP32-footprints-Lib:ESP32-WROOM" H 6350 3550 60  0001 C CNN
F 3 "" H 5550 2650 60  0001 C CNN
	1    6000 2200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Micro_SD_Card_Det J?
U 1 1 5C0021A2
P 9450 2050
F 0 "J?" H 9400 2867 50  0000 C CNN
F 1 "Micro_SD_Card_Det" H 9400 2776 50  0000 C CNN
F 2 "" H 11500 2750 50  0001 C CNN
F 3 "https://www.hirose.com/product/en/download_file/key_name/DM3/category/Catalog/doc_file_id/49662/?file_category_id=4&item_id=195&is_series=1" H 9450 2150 50  0001 C CNN
	1    9450 2050
	1    0    0    -1  
$EndComp
Text GLabel 7950 1650 0    50   Input ~ 0
SD_D2
Text GLabel 7950 1750 0    50   Input ~ 0
SD_D3
Text GLabel 7950 1850 0    50   Input ~ 0
CD_CMD
Text GLabel 7950 1950 0    50   Input ~ 0
VDD
Text GLabel 7950 2050 0    50   Input ~ 0
SD_CLK
Text GLabel 7950 2150 0    50   Input ~ 0
GND
Text GLabel 7950 2250 0    50   Input ~ 0
SD_D0
Text GLabel 7950 2350 0    50   Input ~ 0
SD_D1
Text GLabel 5000 2700 0    50   Input ~ 0
SD_CLK
Text GLabel 6350 3300 3    50   Input ~ 0
CD_CMD
Text GLabel 6450 3300 3    50   Input ~ 0
SD_D0
Text GLabel 6950 2550 2    50   Input ~ 0
SD_D1
Text GLabel 5000 2800 0    50   Input ~ 0
SD_D2
Text GLabel 5650 3300 3    50   Input ~ 0
SD_D3
Text GLabel 5550 3300 3    50   Input ~ 0
GND
Text GLabel 5000 1600 0    50   Input ~ 0
VDD
Text GLabel 10300 2550 2    50   Input ~ 0
GND
Wire Wire Line
	8550 1650 7950 1650
Wire Wire Line
	7950 1750 8000 1750
Wire Wire Line
	8550 1850 8100 1850
Wire Wire Line
	7950 1950 8550 1950
Wire Wire Line
	8550 2050 8200 2050
Wire Wire Line
	8550 2150 7950 2150
Wire Wire Line
	8550 2250 8300 2250
Wire Wire Line
	7950 2350 8400 2350
$Comp
L Device:R R?
U 1 1 5C002AAE
P 8100 1350
F 0 "R?" H 8000 1750 50  0000 L CNN
F 1 "10k" H 8000 1650 50  0000 L CNN
F 2 "" V 8030 1350 50  0001 C CNN
F 3 "~" H 8100 1350 50  0001 C CNN
	1    8100 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002BEA
P 8200 1350
F 0 "R?" H 8150 1750 50  0000 L CNN
F 1 "10k" H 8150 1650 50  0000 L CNN
F 2 "" V 8130 1350 50  0001 C CNN
F 3 "~" H 8200 1350 50  0001 C CNN
	1    8200 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002C3E
P 8300 1350
F 0 "R?" H 8300 1750 50  0000 L CNN
F 1 "10k" H 8300 1650 50  0000 L CNN
F 2 "" V 8230 1350 50  0001 C CNN
F 3 "~" H 8300 1350 50  0001 C CNN
	1    8300 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002C5A
P 8400 1350
F 0 "R?" H 8500 1400 50  0000 L CNN
F 1 "10k" H 8500 1300 50  0000 L CNN
F 2 "" V 8330 1350 50  0001 C CNN
F 3 "~" H 8400 1350 50  0001 C CNN
	1    8400 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C0033AB
P 8000 1350
F 0 "R?" H 7800 1400 50  0000 L CNN
F 1 "10k" H 7800 1300 50  0000 L CNN
F 2 "" V 7930 1350 50  0001 C CNN
F 3 "~" H 8000 1350 50  0001 C CNN
	1    8000 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 1200 8000 1150
Wire Wire Line
	8000 1150 8100 1150
Wire Wire Line
	8400 1150 8400 1200
Wire Wire Line
	8300 1200 8300 1150
Connection ~ 8300 1150
Wire Wire Line
	8300 1150 8400 1150
Wire Wire Line
	8200 1200 8200 1150
Connection ~ 8200 1150
Wire Wire Line
	8200 1150 8300 1150
Wire Wire Line
	8100 1200 8100 1150
Connection ~ 8100 1150
Wire Wire Line
	8100 1150 8200 1150
Text GLabel 7950 1150 0    50   Input ~ 0
VDD
Wire Wire Line
	7950 1150 8000 1150
Connection ~ 8000 1150
Wire Wire Line
	8000 1500 8000 1750
Connection ~ 8000 1750
Wire Wire Line
	8000 1750 8550 1750
Wire Wire Line
	8100 1500 8100 1850
Connection ~ 8100 1850
Wire Wire Line
	8100 1850 7950 1850
Wire Wire Line
	8200 1500 8200 2050
Connection ~ 8200 2050
Wire Wire Line
	8200 2050 7950 2050
Wire Wire Line
	8300 1500 8300 2250
Connection ~ 8300 2250
Wire Wire Line
	8300 2250 7950 2250
Wire Wire Line
	8400 1500 8400 2350
Connection ~ 8400 2350
Wire Wire Line
	8400 2350 8550 2350
Wire Wire Line
	6900 2550 6950 2550
Wire Wire Line
	6450 3300 6450 3250
Wire Wire Line
	6350 3300 6350 3250
Wire Wire Line
	5650 3300 5650 3250
Wire Wire Line
	5550 3300 5550 3250
Wire Wire Line
	5000 2800 5050 2800
Wire Wire Line
	5000 2700 5050 2700
Wire Wire Line
	5000 1600 5050 1600
Text GLabel 5000 2900 0    50   Input ~ 0
GND
Text GLabel 6950 2850 2    50   Input ~ 0
GND
Wire Wire Line
	6900 2850 6950 2850
Wire Wire Line
	5000 2900 5050 2900
$Comp
L vs1011e:VS1011E U?
U 1 1 5C00AA64
P 3450 5050
F 0 "U?" H 3425 6075 50  0000 C CNN
F 1 "VS1011E" H 3425 5984 50  0000 C CNN
F 2 "Housings_QFP:LQFP-48_7x7mm_Pitch0.5mm" H 3450 4150 50  0001 C CNN
F 3 "/home/oleg/projects/rmpd_player/doc/vlsi/vs1011.pdf" H 3050 5400 50  0001 C CNN
	1    3450 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	10300 2550 10250 2550
$Comp
L Device:Crystal Y?
U 1 1 5C010570
P 1750 5350
F 0 "Y?" V 1704 5481 50  0000 L CNN
F 1 "12.288MHz" V 1795 5481 50  0000 L CNN
F 2 "" H 1750 5350 50  0001 C CNN
F 3 "~" H 1750 5350 50  0001 C CNN
	1    1750 5350
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0106D2
P 1550 5350
F 0 "R?" H 1550 5700 50  0000 L CNN
F 1 "1M" H 1550 5600 50  0000 L CNN
F 2 "" V 1480 5350 50  0001 C CNN
F 3 "~" H 1550 5350 50  0001 C CNN
	1    1550 5350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5C010B17
P 1300 5150
F 0 "C?" V 1048 5150 50  0000 C CNN
F 1 "33pF" V 1139 5150 50  0000 C CNN
F 2 "" H 1338 5000 50  0001 C CNN
F 3 "~" H 1300 5150 50  0001 C CNN
	1    1300 5150
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5C010C04
P 1300 5550
F 0 "C?" V 1048 5550 50  0000 C CNN
F 1 "33pF" V 1139 5550 50  0000 C CNN
F 2 "" H 1338 5400 50  0001 C CNN
F 3 "~" H 1300 5550 50  0001 C CNN
	1    1300 5550
	0    1    1    0   
$EndComp
Wire Wire Line
	2600 5450 2500 5450
Wire Wire Line
	2500 5450 2500 5550
Wire Wire Line
	2500 5650 2600 5650
Wire Wire Line
	2600 5550 2500 5550
Connection ~ 2500 5550
Wire Wire Line
	2500 5550 2500 5650
Wire Wire Line
	4250 5150 4300 5150
Wire Wire Line
	4300 5250 4250 5250
Wire Wire Line
	1150 5150 1100 5150
Wire Wire Line
	1100 5150 1100 5550
Wire Wire Line
	1100 5550 1150 5550
Wire Wire Line
	1450 5150 1550 5150
Wire Wire Line
	1550 5200 1550 5150
Connection ~ 1550 5150
Wire Wire Line
	1550 5150 1750 5150
Wire Wire Line
	1750 5200 1750 5150
Connection ~ 1750 5150
Wire Wire Line
	1750 5150 2600 5150
Wire Wire Line
	1450 5550 1550 5550
Wire Wire Line
	2300 5550 2300 5250
Wire Wire Line
	2300 5250 2600 5250
Wire Wire Line
	1550 5500 1550 5550
Connection ~ 1550 5550
Wire Wire Line
	1550 5550 1750 5550
Wire Wire Line
	1750 5500 1750 5550
Connection ~ 1750 5550
Wire Wire Line
	1750 5550 2300 5550
Text GLabel 1000 5550 0    50   Input ~ 0
GND
Wire Wire Line
	1000 5550 1100 5550
Connection ~ 1100 5550
Text GLabel 2450 5650 0    50   Input ~ 0
GND
Wire Wire Line
	2500 5650 2450 5650
Connection ~ 2500 5650
Text GLabel 2050 4350 0    50   Input ~ 0
GND
Text GLabel 2550 5050 0    50   Input ~ 0
GND
Wire Wire Line
	2550 5050 2600 5050
Wire Wire Line
	2050 4350 2100 4350
$Comp
L power:GNDA #PWR?
U 1 1 5C027D6A
P 1600 6700
F 0 "#PWR?" H 1600 6450 50  0001 C CNN
F 1 "GNDA" H 1605 6527 50  0000 C CNN
F 2 "" H 1600 6700 50  0001 C CNN
F 3 "" H 1600 6700 50  0001 C CNN
	1    1600 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 5150 4300 5250
$Comp
L power:GND #PWR?
U 1 1 5C02CF95
P 1850 6700
F 0 "#PWR?" H 1850 6450 50  0001 C CNN
F 1 "GND" H 1855 6527 50  0000 C CNN
F 2 "" H 1850 6700 50  0001 C CNN
F 3 "" H 1850 6700 50  0001 C CNN
	1    1850 6700
	1    0    0    -1  
$EndComp
Text GLabel 1850 6350 1    50   Input ~ 0
GND
Text GLabel 1600 6350 1    50   Input ~ 0
GNDA
Wire Wire Line
	1600 6350 1600 6500
Wire Wire Line
	1850 6350 1850 6500
Text GLabel 4300 5850 2    50   Input ~ 0
GNDA
Text GLabel 4350 5150 2    50   Input ~ 0
GNDA
Text GLabel 4300 4850 2    50   Input ~ 0
GNDA
Wire Wire Line
	4300 4850 4250 4850
Connection ~ 4300 5150
Wire Wire Line
	4300 5850 4250 5850
Wire Wire Line
	1600 6500 1850 6500
Connection ~ 1600 6500
Wire Wire Line
	1600 6500 1600 6700
Connection ~ 1850 6500
Wire Wire Line
	1850 6500 1850 6700
Text Notes 950  7050 0    50   ~ 0
Connect GND and GNDA close to VS1011
$Comp
L Device:R R?
U 1 1 5C03EBF3
P 2300 4250
F 0 "R?" V 2500 4250 50  0000 L CNN
F 1 "100k" V 2400 4250 50  0000 L CNN
F 2 "" V 2230 4250 50  0001 C CNN
F 3 "~" H 2300 4250 50  0001 C CNN
	1    2300 4250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2600 4250 2550 4250
Wire Wire Line
	2150 4250 2100 4250
Wire Wire Line
	2100 4250 2100 4350
Connection ~ 2100 4350
Wire Wire Line
	2100 4350 2600 4350
Text GLabel 2550 4150 1    50   Input ~ 0
VS_XRESET
Wire Wire Line
	2550 4150 2550 4250
Connection ~ 2550 4250
Wire Wire Line
	2550 4250 2450 4250
$Comp
L Device:C C?
U 1 1 5C04D1B2
P 4750 5550
F 0 "C?" V 4498 5550 50  0000 C CNN
F 1 "100n" V 4589 5550 50  0000 C CNN
F 2 "" H 4788 5400 50  0001 C CNN
F 3 "~" H 4750 5550 50  0001 C CNN
	1    4750 5550
	0    1    1    0   
$EndComp
Text GLabel 4950 5550 2    50   Input ~ 0
GNDA
Wire Wire Line
	4250 5550 4600 5550
Wire Wire Line
	4900 5550 4950 5550
Text GLabel 4300 5450 2    50   Input ~ 0
VDDA
Text GLabel 4300 5650 2    50   Input ~ 0
VDDA
Wire Wire Line
	4300 5450 4250 5450
Wire Wire Line
	4250 5650 4300 5650
Wire Wire Line
	4350 5150 4300 5150
Text GLabel 2550 4450 0    50   Input ~ 0
VDD
Text GLabel 2550 4950 0    50   Input ~ 0
VDD
Text GLabel 2550 5350 0    50   Input ~ 0
VDD
Text GLabel 4300 4950 2    50   Input ~ 0
VDDA
Text GLabel 4300 4550 2    50   Input ~ 0
VDD
Wire Wire Line
	2600 4950 2550 4950
Wire Wire Line
	2600 4450 2550 4450
Wire Wire Line
	2600 5350 2550 5350
Wire Wire Line
	4300 4950 4250 4950
Wire Wire Line
	4300 4550 4250 4550
$EndSCHEMATC
