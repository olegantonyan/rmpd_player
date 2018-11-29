EESchema Schematic File Version 4
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
P 6400 2300
F 0 "U?" H 6375 3687 60  0000 C CNN
F 1 "ESP32-WROOM" H 6375 3581 60  0000 C CNN
F 2 "ESP32-footprints-Lib:ESP32-WROOM" H 6750 3650 60  0001 C CNN
F 3 "" H 5950 2750 60  0001 C CNN
	1    6400 2300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Micro_SD_Card_Det J?
U 1 1 5C0021A2
P 9850 2150
F 0 "J?" H 9800 2967 50  0000 C CNN
F 1 "Micro_SD_Card_Det" H 9800 2876 50  0000 C CNN
F 2 "" H 11900 2850 50  0001 C CNN
F 3 "https://www.hirose.com/product/en/download_file/key_name/DM3/category/Catalog/doc_file_id/49662/?file_category_id=4&item_id=195&is_series=1" H 9850 2250 50  0001 C CNN
	1    9850 2150
	1    0    0    -1  
$EndComp
Text GLabel 8350 1750 0    50   Input ~ 0
SD_D2
Text GLabel 8350 1850 0    50   Input ~ 0
SD_D3
Text GLabel 8350 1950 0    50   Input ~ 0
CD_CMD
Text GLabel 8350 2050 0    50   Input ~ 0
VDD
Text GLabel 8350 2150 0    50   Input ~ 0
SD_CLK
Text GLabel 8350 2250 0    50   Input ~ 0
GND
Text GLabel 8350 2350 0    50   Input ~ 0
SD_D0
Text GLabel 8350 2450 0    50   Input ~ 0
SD_D1
Text GLabel 5400 2800 0    50   Input ~ 0
SD_CLK
Text GLabel 6750 3400 3    50   Input ~ 0
CD_CMD
Text GLabel 6850 3400 3    50   Input ~ 0
SD_D0
Text GLabel 7350 2650 2    50   Input ~ 0
SD_D1
Text GLabel 5400 2900 0    50   Input ~ 0
SD_D2
Text GLabel 6050 3400 3    50   Input ~ 0
SD_D3
Text GLabel 5950 3400 3    50   Input ~ 0
GND
Text GLabel 5400 1700 0    50   Input ~ 0
VDD
Text GLabel 10650 2650 2    50   Input ~ 0
GND
Wire Wire Line
	8950 1750 8350 1750
Wire Wire Line
	8350 1850 8400 1850
Wire Wire Line
	8950 1950 8500 1950
Wire Wire Line
	8350 2050 8950 2050
Wire Wire Line
	8950 2150 8600 2150
Wire Wire Line
	8950 2250 8350 2250
Wire Wire Line
	8950 2350 8700 2350
Wire Wire Line
	8350 2450 8800 2450
$Comp
L Device:R R?
U 1 1 5C002AAE
P 8500 1450
F 0 "R?" H 8400 1850 50  0000 L CNN
F 1 "10k" H 8400 1750 50  0000 L CNN
F 2 "" V 8430 1450 50  0001 C CNN
F 3 "~" H 8500 1450 50  0001 C CNN
	1    8500 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002BEA
P 8600 1450
F 0 "R?" H 8550 1850 50  0000 L CNN
F 1 "10k" H 8550 1750 50  0000 L CNN
F 2 "" V 8530 1450 50  0001 C CNN
F 3 "~" H 8600 1450 50  0001 C CNN
	1    8600 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002C3E
P 8700 1450
F 0 "R?" H 8700 1850 50  0000 L CNN
F 1 "10k" H 8700 1750 50  0000 L CNN
F 2 "" V 8630 1450 50  0001 C CNN
F 3 "~" H 8700 1450 50  0001 C CNN
	1    8700 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C002C5A
P 8800 1450
F 0 "R?" H 8900 1500 50  0000 L CNN
F 1 "10k" H 8900 1400 50  0000 L CNN
F 2 "" V 8730 1450 50  0001 C CNN
F 3 "~" H 8800 1450 50  0001 C CNN
	1    8800 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5C0033AB
P 8400 1450
F 0 "R?" H 8200 1500 50  0000 L CNN
F 1 "10k" H 8200 1400 50  0000 L CNN
F 2 "" V 8330 1450 50  0001 C CNN
F 3 "~" H 8400 1450 50  0001 C CNN
	1    8400 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 1300 8400 1250
Wire Wire Line
	8400 1250 8500 1250
Wire Wire Line
	8800 1250 8800 1300
Wire Wire Line
	8700 1300 8700 1250
Connection ~ 8700 1250
Wire Wire Line
	8700 1250 8800 1250
Wire Wire Line
	8600 1300 8600 1250
Connection ~ 8600 1250
Wire Wire Line
	8600 1250 8700 1250
Wire Wire Line
	8500 1300 8500 1250
Connection ~ 8500 1250
Wire Wire Line
	8500 1250 8600 1250
Text GLabel 8350 1250 0    50   Input ~ 0
VDD
Wire Wire Line
	8350 1250 8400 1250
Connection ~ 8400 1250
Wire Wire Line
	8400 1600 8400 1850
Connection ~ 8400 1850
Wire Wire Line
	8400 1850 8950 1850
Wire Wire Line
	8500 1600 8500 1950
Connection ~ 8500 1950
Wire Wire Line
	8500 1950 8350 1950
Wire Wire Line
	8600 1600 8600 2150
Connection ~ 8600 2150
Wire Wire Line
	8600 2150 8350 2150
Wire Wire Line
	8700 1600 8700 2350
Connection ~ 8700 2350
Wire Wire Line
	8700 2350 8350 2350
Wire Wire Line
	8800 1600 8800 2450
Connection ~ 8800 2450
Wire Wire Line
	8800 2450 8950 2450
Wire Wire Line
	7300 2650 7350 2650
Wire Wire Line
	6850 3400 6850 3350
Wire Wire Line
	6750 3400 6750 3350
Wire Wire Line
	6050 3400 6050 3350
Wire Wire Line
	5950 3400 5950 3350
Wire Wire Line
	5400 2900 5450 2900
Wire Wire Line
	5400 2800 5450 2800
Wire Wire Line
	5400 1700 5450 1700
Text GLabel 5400 3000 0    50   Input ~ 0
GND
Text GLabel 7350 2950 2    50   Input ~ 0
GND
Wire Wire Line
	7300 2950 7350 2950
Wire Wire Line
	5400 3000 5450 3000
$EndSCHEMATC
