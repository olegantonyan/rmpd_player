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
P 9600 2050
F 0 "J?" H 9550 2867 50  0000 C CNN
F 1 "Micro_SD_Card_Det" H 9550 2776 50  0000 C CNN
F 2 "" H 11650 2750 50  0001 C CNN
F 3 "https://www.hirose.com/product/en/download_file/key_name/DM3/category/Catalog/doc_file_id/49662/?file_category_id=4&item_id=195&is_series=1" H 9600 2150 50  0001 C CNN
	1    9600 2050
	1    0    0    -1  
$EndComp
Text GLabel 8100 1650 0    50   Input ~ 0
SD_D2
Text GLabel 8100 1750 0    50   Input ~ 0
SD_D3
Text GLabel 8100 1850 0    50   Input ~ 0
CD_CMD
Text GLabel 8100 1950 0    50   Input ~ 0
VDD
Text GLabel 8100 2050 0    50   Input ~ 0
SD_CLK
Text GLabel 8100 2150 0    50   Input ~ 0
GND
Text GLabel 8100 2250 0    50   Input ~ 0
SD_D0
Text GLabel 8100 2350 0    50   Input ~ 0
SD_D1
Text GLabel 5000 2700 0    50   Input ~ 0
SD_CLK
Text GLabel 6350 3300 3    50   Input ~ 0
CD_CMD
Text GLabel 6450 3350 3    50   Input ~ 0
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
Text GLabel 10450 2550 2    50   Input ~ 0
GND
Wire Wire Line
	8700 1650 8100 1650
Wire Wire Line
	8100 1750 8150 1750
Wire Wire Line
	8700 1850 8250 1850
Wire Wire Line
	8100 1950 8700 1950
Wire Wire Line
	8700 2050 8350 2050
Wire Wire Line
	8700 2150 8100 2150
Wire Wire Line
	8700 2250 8450 2250
Wire Wire Line
	8100 2350 8550 2350
$Comp
L Device:R R?
U 1 1 5C002AAE
P 8250 1350
F 0 "R?" H 8150 1750 50  0000 L CNN
F 1 "10k" H 8150 1650 50  0000 L CNN
F 2 "" V 8180 1350 50  0001 C CNN
F 3 "~" H 8250 1350 50  0001 C CNN
	1    8250 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002BEA
P 8350 1350
F 0 "R?" H 8300 1750 50  0000 L CNN
F 1 "10k" H 8300 1650 50  0000 L CNN
F 2 "" V 8280 1350 50  0001 C CNN
F 3 "~" H 8350 1350 50  0001 C CNN
	1    8350 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002C3E
P 8450 1350
F 0 "R?" H 8450 1750 50  0000 L CNN
F 1 "10k" H 8450 1650 50  0000 L CNN
F 2 "" V 8380 1350 50  0001 C CNN
F 3 "~" H 8450 1350 50  0001 C CNN
	1    8450 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002C5A
P 8550 1350
F 0 "R?" H 8650 1400 50  0000 L CNN
F 1 "10k" H 8650 1300 50  0000 L CNN
F 2 "" V 8480 1350 50  0001 C CNN
F 3 "~" H 8550 1350 50  0001 C CNN
	1    8550 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C0033AB
P 8150 1350
F 0 "R?" H 7950 1400 50  0000 L CNN
F 1 "10k" H 7950 1300 50  0000 L CNN
F 2 "" V 8080 1350 50  0001 C CNN
F 3 "~" H 8150 1350 50  0001 C CNN
	1    8150 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 1200 8150 1150
Wire Wire Line
	8150 1150 8250 1150
Wire Wire Line
	8550 1150 8550 1200
Wire Wire Line
	8450 1200 8450 1150
Connection ~ 8450 1150
Wire Wire Line
	8450 1150 8550 1150
Wire Wire Line
	8350 1200 8350 1150
Connection ~ 8350 1150
Wire Wire Line
	8350 1150 8450 1150
Wire Wire Line
	8250 1200 8250 1150
Connection ~ 8250 1150
Wire Wire Line
	8250 1150 8350 1150
Text GLabel 8100 1150 0    50   Input ~ 0
VDD
Wire Wire Line
	8100 1150 8150 1150
Connection ~ 8150 1150
Wire Wire Line
	8150 1500 8150 1750
Connection ~ 8150 1750
Wire Wire Line
	8150 1750 8700 1750
Wire Wire Line
	8250 1500 8250 1850
Connection ~ 8250 1850
Wire Wire Line
	8250 1850 8100 1850
Wire Wire Line
	8350 1500 8350 2050
Connection ~ 8350 2050
Wire Wire Line
	8350 2050 8100 2050
Wire Wire Line
	8450 1500 8450 2250
Connection ~ 8450 2250
Wire Wire Line
	8450 2250 8100 2250
Wire Wire Line
	8550 1500 8550 2350
Connection ~ 8550 2350
Wire Wire Line
	8550 2350 8700 2350
Wire Wire Line
	6900 2550 6950 2550
Wire Wire Line
	6450 3350 6450 3300
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
Text GLabel 7000 2850 2    50   Input ~ 0
GND
Wire Wire Line
	6900 2850 6950 2850
Wire Wire Line
	5000 2900 5050 2900
$Comp
L vs1011e:VS1011E U?
U 1 1 5C00AA64
P 3300 4400
F 0 "U?" H 3275 5425 50  0000 C CNN
F 1 "VS1011E" H 3275 5334 50  0000 C CNN
F 2 "Housings_QFP:LQFP-48_7x7mm_Pitch0.5mm" H 3300 3500 50  0001 C CNN
F 3 "/home/oleg/projects/rmpd_player/doc/vlsi/vs1011.pdf" H 2900 4750 50  0001 C CNN
	1    3300 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	10450 2550 10400 2550
$Comp
L Device:Crystal Y?
U 1 1 5C010570
P 1600 4700
F 0 "Y?" V 1554 4831 50  0000 L CNN
F 1 "12.288MHz" V 1645 4831 50  0000 L CNN
F 2 "" H 1600 4700 50  0001 C CNN
F 3 "~" H 1600 4700 50  0001 C CNN
	1    1600 4700
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0106D2
P 1400 4700
F 0 "R?" H 1400 5050 50  0000 L CNN
F 1 "1M" H 1400 4950 50  0000 L CNN
F 2 "" V 1330 4700 50  0001 C CNN
F 3 "~" H 1400 4700 50  0001 C CNN
	1    1400 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5C010B17
P 1150 4500
F 0 "C?" V 898 4500 50  0000 C CNN
F 1 "33pF" V 989 4500 50  0000 C CNN
F 2 "" H 1188 4350 50  0001 C CNN
F 3 "~" H 1150 4500 50  0001 C CNN
	1    1150 4500
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5C010C04
P 1150 4900
F 0 "C?" V 898 4900 50  0000 C CNN
F 1 "33pF" V 989 4900 50  0000 C CNN
F 2 "" H 1188 4750 50  0001 C CNN
F 3 "~" H 1150 4900 50  0001 C CNN
	1    1150 4900
	0    1    1    0   
$EndComp
Wire Wire Line
	2450 4800 2350 4800
Wire Wire Line
	2350 4800 2350 4900
Wire Wire Line
	2350 5000 2450 5000
Wire Wire Line
	2450 4900 2350 4900
Connection ~ 2350 4900
Wire Wire Line
	2350 4900 2350 5000
Wire Wire Line
	4100 4500 4150 4500
Wire Wire Line
	4150 4600 4100 4600
Wire Wire Line
	1000 4500 950  4500
Wire Wire Line
	950  4500 950  4900
Wire Wire Line
	950  4900 1000 4900
Wire Wire Line
	1300 4500 1400 4500
Wire Wire Line
	1400 4550 1400 4500
Connection ~ 1400 4500
Wire Wire Line
	1400 4500 1600 4500
Wire Wire Line
	1600 4550 1600 4500
Connection ~ 1600 4500
Wire Wire Line
	1600 4500 2450 4500
Wire Wire Line
	1300 4900 1400 4900
Wire Wire Line
	2150 4900 2150 4600
Wire Wire Line
	2150 4600 2450 4600
Wire Wire Line
	1400 4850 1400 4900
Connection ~ 1400 4900
Wire Wire Line
	1400 4900 1600 4900
Wire Wire Line
	1600 4850 1600 4900
Connection ~ 1600 4900
Wire Wire Line
	1600 4900 2150 4900
Text GLabel 850  4900 0    50   Input ~ 0
GND
Wire Wire Line
	850  4900 950  4900
Connection ~ 950  4900
Text GLabel 2300 5000 0    50   Input ~ 0
GND
Wire Wire Line
	2350 5000 2300 5000
Connection ~ 2350 5000
Text GLabel 1900 3700 0    50   Input ~ 0
GND
Text GLabel 2400 4400 0    50   Input ~ 0
GND
Wire Wire Line
	2400 4400 2450 4400
Wire Wire Line
	1900 3700 1950 3700
$Comp
L power:GNDA #PWR?
U 1 1 5C027D6A
P 1450 6050
F 0 "#PWR?" H 1450 5800 50  0001 C CNN
F 1 "GNDA" H 1455 5877 50  0000 C CNN
F 2 "" H 1450 6050 50  0001 C CNN
F 3 "" H 1450 6050 50  0001 C CNN
	1    1450 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4500 4150 4600
$Comp
L power:GND #PWR?
U 1 1 5C02CF95
P 1700 6050
F 0 "#PWR?" H 1700 5800 50  0001 C CNN
F 1 "GND" H 1705 5877 50  0000 C CNN
F 2 "" H 1700 6050 50  0001 C CNN
F 3 "" H 1700 6050 50  0001 C CNN
	1    1700 6050
	1    0    0    -1  
$EndComp
Text GLabel 1700 5700 1    50   Input ~ 0
GND
Text GLabel 1450 5700 1    50   Input ~ 0
GNDA
Wire Wire Line
	1450 5700 1450 5850
Wire Wire Line
	1700 5700 1700 5850
Text GLabel 4150 5200 2    50   Input ~ 0
GNDA
Text GLabel 4200 4500 2    50   Input ~ 0
GNDA
Text GLabel 4150 4200 2    50   Input ~ 0
GNDA
Wire Wire Line
	4150 4200 4100 4200
Connection ~ 4150 4500
Wire Wire Line
	4150 5200 4100 5200
Wire Wire Line
	1450 5850 1700 5850
Connection ~ 1450 5850
Wire Wire Line
	1450 5850 1450 6050
Connection ~ 1700 5850
Wire Wire Line
	1700 5850 1700 6050
$Comp
L Device:R R?
U 1 1 5C03EBF3
P 2150 3600
F 0 "R?" V 2350 3600 50  0000 L CNN
F 1 "100k" V 2250 3600 50  0000 L CNN
F 2 "" V 2080 3600 50  0001 C CNN
F 3 "~" H 2150 3600 50  0001 C CNN
	1    2150 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2450 3600 2400 3600
Wire Wire Line
	2000 3600 1950 3600
Wire Wire Line
	1950 3600 1950 3700
Connection ~ 1950 3700
Wire Wire Line
	1950 3700 2450 3700
Text GLabel 2400 3500 1    50   Input ~ 0
VS_XRESET
Wire Wire Line
	2400 3500 2400 3600
Connection ~ 2400 3600
Wire Wire Line
	2400 3600 2300 3600
$Comp
L Device:C C?
U 1 1 5C04D1B2
P 4450 5750
F 0 "C?" H 4550 5850 50  0000 C CNN
F 1 "100n" H 4550 5650 50  0000 C CNN
F 2 "" H 4488 5600 50  0001 C CNN
F 3 "~" H 4450 5750 50  0001 C CNN
	1    4450 5750
	1    0    0    -1  
$EndComp
Text GLabel 4450 6000 3    50   Input ~ 0
GNDA
Text GLabel 4150 4800 2    50   Input ~ 0
VDDA
Text GLabel 4150 5000 2    50   Input ~ 0
VDDA
Wire Wire Line
	4150 4800 4100 4800
Wire Wire Line
	4100 5000 4150 5000
Wire Wire Line
	4200 4500 4150 4500
Text GLabel 2400 3800 0    50   Input ~ 0
VDD
Text GLabel 2400 4300 0    50   Input ~ 0
VDD
Text GLabel 2400 4700 0    50   Input ~ 0
VDD
Text GLabel 4150 4300 2    50   Input ~ 0
VDDA
Text GLabel 4150 3900 2    50   Input ~ 0
VDD
Wire Wire Line
	2450 4300 2400 4300
Wire Wire Line
	2450 3800 2400 3800
Wire Wire Line
	2450 4700 2400 4700
Wire Wire Line
	4150 4300 4100 4300
Wire Wire Line
	4150 3900 4100 3900
Text GLabel 4150 3600 2    50   Input ~ 0
VS_SCLK
Text GLabel 4150 3700 2    50   Input ~ 0
VS_SI
Text GLabel 4150 3800 2    50   Input ~ 0
VS_SO
Wire Wire Line
	4150 3600 4100 3600
Wire Wire Line
	4150 3700 4100 3700
Wire Wire Line
	4150 3800 4100 3800
Text GLabel 2400 3900 0    50   Input ~ 0
VS_DREQ
Wire Wire Line
	2450 3900 2400 3900
Text GLabel 2400 4200 0    50   Input ~ 0
VS_XDCS
Wire Wire Line
	2450 4200 2400 4200
Text GLabel 2400 5100 0    50   Input ~ 0
VS_XCS
Wire Wire Line
	2450 5100 2400 5100
Wire Wire Line
	6900 2750 6950 2750
Wire Wire Line
	6950 2750 6950 2850
Connection ~ 6950 2850
Wire Wire Line
	6950 2850 7000 2850
$Comp
L Device:R R?
U 1 1 5C090195
P 4400 1500
F 0 "R?" H 4200 1550 50  0000 L CNN
F 1 "10k" H 4200 1450 50  0000 L CNN
F 2 "" V 4330 1500 50  0001 C CNN
F 3 "~" H 4400 1500 50  0001 C CNN
	1    4400 1500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5C0902B0
P 4400 1900
F 0 "C?" H 4550 1850 50  0000 C CNN
F 1 "100n" H 4600 1950 50  0000 C CNN
F 2 "" H 4438 1750 50  0001 C CNN
F 3 "~" H 4400 1900 50  0001 C CNN
	1    4400 1900
	-1   0    0    1   
$EndComp
Text GLabel 4400 2100 3    50   Input ~ 0
GND
Text GLabel 4400 1300 1    50   Input ~ 0
VDD
Wire Wire Line
	4400 1300 4400 1350
Wire Wire Line
	4400 2100 4400 2050
Wire Wire Line
	4400 1650 4400 1700
Connection ~ 4400 1700
Wire Wire Line
	4400 1700 4400 1750
Wire Wire Line
	6900 2650 7250 2650
Wire Wire Line
	7250 2650 7250 3300
Wire Wire Line
	7250 3300 6450 3300
Connection ~ 6450 3300
Wire Wire Line
	6450 3300 6450 3250
$Comp
L Device:C C?
U 1 1 5C0BA04F
P 4750 5750
F 0 "C?" H 4650 5650 50  0000 C CNN
F 1 "47n" H 4650 5850 50  0000 C CNN
F 2 "" H 4788 5600 50  0001 C CNN
F 3 "~" H 4750 5750 50  0001 C CNN
	1    4750 5750
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0D5425
P 4750 5400
F 0 "R?" H 4850 5450 50  0000 L CNN
F 1 "10" H 4850 5350 50  0000 L CNN
F 2 "" V 4680 5400 50  0001 C CNN
F 3 "~" H 4750 5400 50  0001 C CNN
	1    4750 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 4900 4450 4900
Wire Wire Line
	4450 4900 4450 5600
Wire Wire Line
	4450 5900 4450 5950
Wire Wire Line
	4100 4700 4750 4700
Wire Wire Line
	4750 4700 4750 5250
Wire Wire Line
	4750 5550 4750 5600
Wire Wire Line
	4450 5950 4750 5950
Wire Wire Line
	4750 5950 4750 5900
Connection ~ 4450 5950
Wire Wire Line
	4450 5950 4450 6000
$Comp
L Device:R R?
U 1 1 5C0F9901
P 5050 5400
F 0 "R?" H 5150 5450 50  0000 L CNN
F 1 "20" H 5150 5350 50  0000 L CNN
F 2 "" V 4980 5400 50  0001 C CNN
F 3 "~" H 5050 5400 50  0001 C CNN
	1    5050 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C0F9A2F
P 5350 5400
F 0 "R?" H 5450 5450 50  0000 L CNN
F 1 "20" H 5450 5350 50  0000 L CNN
F 2 "" V 5280 5400 50  0001 C CNN
F 3 "~" H 5350 5400 50  0001 C CNN
	1    5350 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5C0F9AF1
P 5050 5750
F 0 "C?" H 4950 5650 50  0000 C CNN
F 1 "10n" H 4950 5850 50  0000 C CNN
F 2 "" H 5088 5600 50  0001 C CNN
F 3 "~" H 5050 5750 50  0001 C CNN
	1    5050 5750
	-1   0    0    1   
$EndComp
$Comp
L Device:C C?
U 1 1 5C10A88A
P 5350 5750
F 0 "C?" H 5250 5650 50  0000 C CNN
F 1 "10n" H 5250 5850 50  0000 C CNN
F 2 "" H 5388 5600 50  0001 C CNN
F 3 "~" H 5350 5750 50  0001 C CNN
	1    5350 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	5050 5550 5050 5600
Wire Wire Line
	5350 5550 5350 5600
Wire Wire Line
	4750 5950 5050 5950
Wire Wire Line
	5050 5950 5050 5900
Connection ~ 4750 5950
Wire Wire Line
	5350 5900 5350 5950
Wire Wire Line
	5350 5950 5050 5950
Connection ~ 5050 5950
Wire Wire Line
	5050 5100 5050 5250
Wire Wire Line
	4100 4400 5350 4400
Wire Wire Line
	5350 4400 5350 5250
Wire Wire Line
	4100 5100 5050 5100
$Comp
L Device:R R?
U 1 1 5C18D338
P 5550 4400
F 0 "R?" V 5750 4400 50  0000 L CNN
F 1 "470" V 5650 4400 50  0000 L CNN
F 2 "" V 5480 4400 50  0001 C CNN
F 3 "~" H 5550 4400 50  0001 C CNN
	1    5550 4400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C18D432
P 5550 5100
F 0 "R?" V 5750 5100 50  0000 L CNN
F 1 "470" V 5650 5100 50  0000 L CNN
F 2 "" V 5480 5100 50  0001 C CNN
F 3 "~" H 5550 5100 50  0001 C CNN
	1    5550 5100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 4400 5350 4400
Connection ~ 5350 4400
Wire Wire Line
	5050 5100 5400 5100
Connection ~ 5050 5100
$Comp
L Device:C C?
U 1 1 5C1AAC76
P 6050 5750
F 0 "C?" H 5950 5650 50  0000 C CNN
F 1 "3n3" H 5950 5850 50  0000 C CNN
F 2 "" H 6088 5600 50  0001 C CNN
F 3 "~" H 6050 5750 50  0001 C CNN
	1    6050 5750
	-1   0    0    1   
$EndComp
$Comp
L Device:C C?
U 1 1 5C1AAD44
P 5750 5750
F 0 "C?" H 5650 5650 50  0000 C CNN
F 1 "3n3" H 5650 5850 50  0000 C CNN
F 2 "" H 5788 5600 50  0001 C CNN
F 3 "~" H 5750 5750 50  0001 C CNN
	1    5750 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	5700 4400 6050 4400
Wire Wire Line
	6050 4400 6050 5600
Wire Wire Line
	5700 5100 5750 5100
Wire Wire Line
	5750 5100 5750 5600
Wire Wire Line
	5350 5950 5750 5950
Wire Wire Line
	5750 5950 5750 5900
Connection ~ 5350 5950
Wire Wire Line
	5750 5950 6050 5950
Wire Wire Line
	6050 5950 6050 5900
Connection ~ 5750 5950
$Comp
L Device:CP C?
U 1 1 5C28C12B
P 6250 4400
F 0 "C?" V 6505 4400 50  0000 C CNN
F 1 "10.0" V 6414 4400 50  0000 C CNN
F 2 "" H 6288 4250 50  0001 C CNN
F 3 "~" H 6250 4400 50  0001 C CNN
	1    6250 4400
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP C?
U 1 1 5C28C2DD
P 6250 5100
F 0 "C?" V 6505 5100 50  0000 C CNN
F 1 "10.0" V 6414 5100 50  0000 C CNN
F 2 "" H 6288 4950 50  0001 C CNN
F 3 "~" H 6250 5100 50  0001 C CNN
	1    6250 5100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6100 5100 5750 5100
Connection ~ 5750 5100
Wire Wire Line
	6100 4400 6050 4400
Connection ~ 6050 4400
$Comp
L Device:R R?
U 1 1 5C29D72C
P 6450 5450
F 0 "R?" H 6250 5450 50  0000 L CNN
F 1 "10k" H 6250 5350 50  0000 L CNN
F 2 "" V 6380 5450 50  0001 C CNN
F 3 "~" H 6450 5450 50  0001 C CNN
	1    6450 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C29D83E
P 6600 5450
F 0 "R?" H 6700 5500 50  0000 L CNN
F 1 "10k" H 6700 5400 50  0000 L CNN
F 2 "" V 6530 5450 50  0001 C CNN
F 3 "~" H 6600 5450 50  0001 C CNN
	1    6600 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 5100 6450 5100
Wire Wire Line
	6450 5100 6450 5300
Wire Wire Line
	6050 5950 6450 5950
Wire Wire Line
	6450 5950 6450 5600
Connection ~ 6050 5950
Wire Wire Line
	6400 4400 6600 4400
Wire Wire Line
	6600 4400 6600 5300
Wire Wire Line
	6600 5600 6600 5950
Wire Wire Line
	6600 5950 6450 5950
Connection ~ 6450 5950
$Comp
L Connector:AudioJack3_Ground J?
U 1 1 5C2C1C35
P 7150 4800
F 0 "J?" H 7400 4500 50  0000 R CNN
F 1 "AudioJack3_Ground" H 7250 4500 50  0000 R CNN
F 2 "" H 7150 4800 50  0001 C CNN
F 3 "~" H 7150 4800 50  0001 C CNN
	1    7150 4800
	-1   0    0    1   
$EndComp
Wire Wire Line
	6600 4400 6800 4400
Wire Wire Line
	6800 4400 6800 4700
Wire Wire Line
	6800 4700 6950 4700
Connection ~ 6600 4400
Wire Wire Line
	6450 5100 6800 5100
Wire Wire Line
	6800 5100 6800 4800
Wire Wire Line
	6800 4800 6950 4800
Connection ~ 6450 5100
Wire Wire Line
	6600 5950 6900 5950
Wire Wire Line
	6900 5950 6900 4900
Wire Wire Line
	6900 4900 6950 4900
Connection ~ 6600 5950
$Comp
L Device:R R?
U 1 1 5C020EC9
P 1750 4000
F 0 "R?" V 1850 4050 50  0000 L CNN
F 1 "100k" V 1850 3850 50  0000 L CNN
F 2 "" V 1680 4000 50  0001 C CNN
F 3 "~" H 1750 4000 50  0001 C CNN
	1    1750 4000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C021253
P 1750 4100
F 0 "R?" V 1650 4150 50  0000 L CNN
F 1 "100k" V 1650 3950 50  0000 L CNN
F 2 "" V 1680 4100 50  0001 C CNN
F 3 "~" H 1750 4100 50  0001 C CNN
	1    1750 4100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2450 4000 1900 4000
Wire Wire Line
	2450 4100 1900 4100
Wire Wire Line
	1600 4000 1550 4000
Wire Wire Line
	1550 4000 1550 4100
Wire Wire Line
	1550 4100 1600 4100
Text GLabel 1500 4000 0    50   Input ~ 0
GND
Wire Wire Line
	1500 4000 1550 4000
Connection ~ 1550 4000
$Comp
L Device:R R?
U 1 1 5C03E022
P 4650 4000
F 0 "R?" V 4550 3900 50  0000 L CNN
F 1 "100k" V 4550 4000 50  0000 L CNN
F 2 "" V 4580 4000 50  0001 C CNN
F 3 "~" H 4650 4000 50  0001 C CNN
	1    4650 4000
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C03E122
P 4650 4100
F 0 "R?" V 4750 4000 50  0000 L CNN
F 1 "100k" V 4750 4100 50  0000 L CNN
F 2 "" V 4580 4100 50  0001 C CNN
F 3 "~" H 4650 4100 50  0001 C CNN
	1    4650 4100
	0    1    1    0   
$EndComp
Text GLabel 4900 4000 2    50   Input ~ 0
GND
Wire Wire Line
	4100 4000 4500 4000
Wire Wire Line
	4500 4100 4100 4100
Wire Wire Line
	4800 4000 4850 4000
Wire Wire Line
	4800 4100 4850 4100
Wire Wire Line
	4850 4100 4850 4000
Connection ~ 4850 4000
Wire Wire Line
	4850 4000 4900 4000
Text GLabel 6950 2250 2    50   Input ~ 0
VS_XCS
Wire Wire Line
	6950 2250 6900 2250
Text GLabel 6950 2150 2    50   Input ~ 0
VS_SCLK
Text GLabel 6950 2050 2    50   Input ~ 0
VS_SO
Text GLabel 6950 1450 2    50   Input ~ 0
VS_SI
Wire Wire Line
	6950 1450 6900 1450
Wire Wire Line
	6950 2050 6900 2050
Wire Wire Line
	6950 2150 6900 2150
Text GLabel 6950 1850 2    50   Input ~ 0
VS_XRESET
Text GLabel 6950 1550 2    50   Input ~ 0
VS_DREQ
Text GLabel 6950 2350 2    50   Input ~ 0
VS_XDCS
Wire Wire Line
	6950 1550 6900 1550
Wire Wire Line
	6950 1850 6900 1850
Wire Wire Line
	6950 2350 6900 2350
Text GLabel 2600 5750 0    50   Input ~ 0
VDDA
Text GLabel 3550 5750 2    50   Input ~ 0
VDD
$Comp
L Device:C C?
U 1 1 5C06DB73
P 2700 5950
F 0 "C?" H 2800 6050 50  0000 C CNN
F 1 "100n" H 2800 5850 50  0000 C CNN
F 2 "" H 2738 5800 50  0001 C CNN
F 3 "~" H 2700 5950 50  0001 C CNN
	1    2700 5950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5C06DC75
P 3450 5950
F 0 "C?" H 3550 6050 50  0000 C CNN
F 1 "100n" H 3550 5850 50  0000 C CNN
F 2 "" H 3488 5800 50  0001 C CNN
F 3 "~" H 3450 5950 50  0001 C CNN
	1    3450 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 5750 2700 5750
Wire Wire Line
	3450 5800 3450 5750
Connection ~ 3450 5750
Wire Wire Line
	3450 5750 3550 5750
Wire Wire Line
	2700 5800 2700 5750
Connection ~ 2700 5750
Wire Wire Line
	2700 5750 2950 5750
Wire Wire Line
	3450 6100 3450 6150
Wire Wire Line
	2700 6100 2700 6150
Wire Wire Line
	4400 1700 5050 1700
Text GLabel 3450 6150 3    50   Input ~ 0
GND
Text GLabel 2700 6150 3    50   Input ~ 0
GNDA
$Comp
L Transistor_BJT:BC848 Q?
U 1 1 5C055DAE
P 8000 4700
F 0 "Q?" H 8190 4746 50  0000 L CNN
F 1 "BC848" H 8190 4655 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8200 4625 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/Infineon-BC847SERIES_BC848SERIES_BC849SERIES_BC850SERIES-DS-v01_01-en.pdf?fileId=db3a304314dca389011541d4630a1657" H 8000 4700 50  0001 L CNN
	1    8000 4700
	-1   0    0    -1  
$EndComp
$Comp
L Transistor_BJT:BC848 Q?
U 1 1 5C056099
P 8400 5600
F 0 "Q?" H 8590 5646 50  0000 L CNN
F 1 "BC848" H 8590 5555 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8600 5525 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/Infineon-BC847SERIES_BC848SERIES_BC849SERIES_BC850SERIES-DS-v01_01-en.pdf?fileId=db3a304314dca389011541d4630a1657" H 8400 5600 50  0001 L CNN
	1    8400 5600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6800 4400 7900 4400
Wire Wire Line
	7900 4400 7900 4500
Connection ~ 6800 4400
Wire Wire Line
	8300 5400 8300 5100
Wire Wire Line
	8300 5100 6800 5100
Connection ~ 6800 5100
Wire Wire Line
	7900 5950 6900 5950
Connection ~ 6900 5950
Wire Wire Line
	8300 5800 8300 5950
Wire Wire Line
	8300 5950 7900 5950
Connection ~ 7900 5950
Wire Wire Line
	7900 4900 7900 5950
$Comp
L Device:R R?
U 1 1 5C0BA812
P 8400 4700
F 0 "R?" V 8300 4700 50  0000 L CNN
F 1 "10k" V 8500 4650 50  0000 L CNN
F 2 "" V 8330 4700 50  0001 C CNN
F 3 "~" H 8400 4700 50  0001 C CNN
	1    8400 4700
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0BAA8F
P 8800 5600
F 0 "R?" V 8700 5600 50  0000 L CNN
F 1 "10k" V 8900 5550 50  0000 L CNN
F 2 "" V 8730 5600 50  0001 C CNN
F 3 "~" H 8800 5600 50  0001 C CNN
	1    8800 5600
	0    1    1    0   
$EndComp
$Comp
L Transistor_BJT:BC858 Q?
U 1 1 5C0BAC2A
P 8700 4400
F 0 "Q?" H 8890 4354 50  0000 L CNN
F 1 "BC858" H 8890 4445 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 8900 4325 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC856.pdf" H 8700 4400 50  0001 L CNN
	1    8700 4400
	-1   0    0    1   
$EndComp
$Comp
L Transistor_BJT:BC858 Q?
U 1 1 5C0BADDA
P 9100 5300
F 0 "Q?" H 9290 5254 50  0000 L CNN
F 1 "BC858" H 9290 5345 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 9300 5225 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC856.pdf" H 9100 5300 50  0001 L CNN
	1    9100 5300
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0BB062
P 9700 4200
F 0 "R?" H 9800 4150 50  0000 L CNN
F 1 "100k" H 9750 4250 50  0000 L CNN
F 2 "" V 9630 4200 50  0001 C CNN
F 3 "~" H 9700 4200 50  0001 C CNN
	1    9700 4200
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0BB3CA
P 9300 4400
F 0 "R?" V 9200 4400 50  0000 L CNN
F 1 "10k" V 9400 4350 50  0000 L CNN
F 2 "" V 9230 4400 50  0001 C CNN
F 3 "~" H 9300 4400 50  0001 C CNN
	1    9300 4400
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5C0BB458
P 9500 5300
F 0 "R?" V 9400 5300 50  0000 L CNN
F 1 "10k" V 9600 5250 50  0000 L CNN
F 2 "" V 9430 5300 50  0001 C CNN
F 3 "~" H 9500 5300 50  0001 C CNN
	1    9500 5300
	0    1    1    0   
$EndComp
Wire Wire Line
	8900 4400 9150 4400
Wire Wire Line
	9450 4400 9700 4400
Wire Wire Line
	9700 4400 9700 5300
Wire Wire Line
	9700 5300 9650 5300
Wire Wire Line
	9700 4350 9700 4400
Connection ~ 9700 4400
Wire Wire Line
	8550 4700 8600 4700
Wire Wire Line
	8600 4700 8600 4600
Wire Wire Line
	8950 5600 9000 5600
Wire Wire Line
	9000 5600 9000 5500
Wire Wire Line
	8600 5600 8650 5600
Wire Wire Line
	8200 4700 8250 4700
Wire Wire Line
	8600 4200 8600 4000
Wire Wire Line
	8600 4000 9000 4000
Wire Wire Line
	9700 4000 9700 4050
Wire Wire Line
	9000 5100 9000 4000
Connection ~ 9000 4000
Wire Wire Line
	9000 4000 9700 4000
Text GLabel 9000 3950 1    50   Input ~ 0
VDD
Wire Wire Line
	9000 4000 9000 3950
Wire Wire Line
	9350 5300 9300 5300
Text GLabel 9800 4400 2    50   Input ~ 0
TRANSIENT_MUTE
Wire Wire Line
	9800 4400 9700 4400
Text GLabel 6950 2450 2    50   Input ~ 0
TRANSIENT_MUTE
Wire Wire Line
	6900 2450 6950 2450
$Comp
L Device:C C?
U 1 1 5C08CC4A
P 2950 5950
F 0 "C?" H 3050 6050 50  0000 C CNN
F 1 "100n" H 3050 5850 50  0000 C CNN
F 2 "" H 2988 5800 50  0001 C CNN
F 3 "~" H 2950 5950 50  0001 C CNN
	1    2950 5950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5C08CCC0
P 3200 5950
F 0 "C?" H 3300 6050 50  0000 C CNN
F 1 "100n" H 3300 5850 50  0000 C CNN
F 2 "" H 3238 5800 50  0001 C CNN
F 3 "~" H 3200 5950 50  0001 C CNN
	1    3200 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 5800 2950 5750
Connection ~ 2950 5750
Wire Wire Line
	2950 5750 3200 5750
Wire Wire Line
	3200 5800 3200 5750
Connection ~ 3200 5750
Wire Wire Line
	3200 5750 3450 5750
Text GLabel 2950 6150 3    50   Input ~ 0
GNDA
Text GLabel 3200 6150 3    50   Input ~ 0
GND
Wire Wire Line
	2950 6100 2950 6150
Wire Wire Line
	3200 6100 3200 6150
$Comp
L Regulator_Linear:LM1117-3.3 U?
U 1 1 5C0BDAEC
P 5450 6700
F 0 "U?" H 5450 6942 50  0000 C CNN
F 1 "LM1117-3.3" H 5450 6851 50  0000 C CNN
F 2 "" H 5450 6700 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm1117.pdf" H 5450 6700 50  0001 C CNN
	1    5450 6700
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C?
U 1 1 5C0BE156
P 5050 6950
F 0 "C?" H 4850 7000 50  0000 C CNN
F 1 "10.0" H 4850 6900 50  0000 C CNN
F 2 "" H 5088 6800 50  0001 C CNN
F 3 "~" H 5050 6950 50  0001 C CNN
	1    5050 6950
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C?
U 1 1 5C0BE7B4
P 5850 6950
F 0 "C?" H 6050 7000 50  0000 C CNN
F 1 "10.0" H 6050 6900 50  0000 C CNN
F 2 "" H 5888 6800 50  0001 C CNN
F 3 "~" H 5850 6950 50  0001 C CNN
	1    5850 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 6700 5050 6700
Wire Wire Line
	5050 6700 5050 6800
Wire Wire Line
	5050 7100 5050 7150
Wire Wire Line
	5050 7150 5450 7150
Wire Wire Line
	5850 7150 5850 7100
Wire Wire Line
	5450 7000 5450 7150
Connection ~ 5450 7150
Wire Wire Line
	5450 7150 5850 7150
Wire Wire Line
	5750 6700 5850 6700
Wire Wire Line
	5850 6700 5850 6800
Text GLabel 5450 7200 3    50   Input ~ 0
GND
Wire Wire Line
	5450 7200 5450 7150
Text GLabel 6000 6700 2    50   Input ~ 0
VDD
Text GLabel 6000 6600 2    50   Input ~ 0
VDDA
Wire Wire Line
	6000 6600 5850 6600
Wire Wire Line
	5850 6600 5850 6700
Connection ~ 5850 6700
Wire Wire Line
	5850 6700 6000 6700
$Comp
L Connector:Barrel_Jack J?
U 1 1 5C11E4C3
P 3950 6800
F 0 "J?" H 4005 7125 50  0000 C CNN
F 1 "Barrel_Jack" H 4005 7034 50  0000 C CNN
F 2 "" H 4000 6760 50  0001 C CNN
F 3 "~" H 4000 6760 50  0001 C CNN
	1    3950 6800
	1    0    0    -1  
$EndComp
Connection ~ 5050 6700
Wire Wire Line
	4250 6900 4300 6900
Wire Wire Line
	4300 6900 4300 7150
Connection ~ 5050 7150
Wire Wire Line
	4250 6700 4350 6700
Wire Wire Line
	4300 7150 4700 7150
$Comp
L Device:Fuse F?
U 1 1 5C16C081
P 4500 6700
F 0 "F?" V 4697 6700 50  0000 C CNN
F 1 "Fuse" V 4606 6700 50  0000 C CNN
F 2 "" V 4430 6700 50  0001 C CNN
F 3 "~" H 4500 6700 50  0001 C CNN
	1    4500 6700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4650 6700 4700 6700
$Comp
L Device:D_Zener D?
U 1 1 5C19A59E
P 4700 6950
F 0 "D?" V 4650 6750 50  0000 L CNN
F 1 "5.1V" V 4750 6700 50  0000 L CNN
F 2 "" H 4700 6950 50  0001 C CNN
F 3 "~" H 4700 6950 50  0001 C CNN
	1    4700 6950
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 6700 4700 6800
Connection ~ 4700 6700
Wire Wire Line
	4700 6700 5050 6700
Wire Wire Line
	4700 7100 4700 7150
Connection ~ 4700 7150
Wire Wire Line
	4700 7150 5050 7150
$Comp
L Timer_RTC:DS3231M U?
U 1 1 5C087D5D
P 2250 1700
F 0 "U?" H 1950 1350 50  0000 C CNN
F 1 "DS3231M" H 1950 1250 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 2250 1100 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS3231.pdf" H 2520 1750 50  0001 C CNN
	1    2250 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT?
U 1 1 5C0886E0
P 2550 1200
F 0 "BT?" V 2805 1250 50  0000 C CNN
F 1 "CR2032" V 2714 1250 50  0000 C CNN
F 2 "" V 2550 1260 50  0001 C CNN
F 3 "~" V 2550 1260 50  0001 C CNN
	1    2550 1200
	0    -1   -1   0   
$EndComp
Text GLabel 2700 1200 2    50   Input ~ 0
GND
Text GLabel 2250 2150 3    50   Input ~ 0
GND
Text GLabel 2150 1100 1    50   Input ~ 0
VDD
Wire Wire Line
	2150 1300 2150 1200
Wire Wire Line
	2250 1300 2250 1200
Wire Wire Line
	2250 1200 2350 1200
Wire Wire Line
	2700 1200 2650 1200
Wire Wire Line
	2250 2150 2250 2100
Text GLabel 1650 1500 0    50   Input ~ 0
I2C_SCL
Text GLabel 1650 1600 0    50   Input ~ 0
I2C_SDA
Wire Wire Line
	1750 1500 1650 1500
Wire Wire Line
	1650 1600 1750 1600
Text GLabel 5000 2600 0    50   Input ~ 0
I2C_SDA
Text GLabel 5000 2500 0    50   Input ~ 0
I2C_SCL
Wire Wire Line
	5050 2500 5000 2500
Wire Wire Line
	5000 2600 5050 2600
$Comp
L Device:C C?
U 1 1 5C11BEA3
P 1950 1200
F 0 "C?" V 2200 1200 50  0000 C CNN
F 1 "100n" V 2100 1200 50  0000 C CNN
F 2 "" H 1988 1050 50  0001 C CNN
F 3 "~" H 1950 1200 50  0001 C CNN
	1    1950 1200
	0    -1   -1   0   
$EndComp
Text GLabel 1750 1200 0    50   Input ~ 0
GND
Wire Wire Line
	1800 1200 1750 1200
Wire Wire Line
	2100 1200 2150 1200
Connection ~ 2150 1200
Wire Wire Line
	2150 1200 2150 1100
$EndSCHEMATC
