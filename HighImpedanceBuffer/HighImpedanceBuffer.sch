EESchema Schematic File Version 4
LIBS:HighImpedanceBuffer-cache
EELAYER 29 0
EELAYER END
$Descr USLetter 11000 8500
encoding utf-8
Sheet 1 1
Title "High Impedance Buffer"
Date "2019-10-01"
Rev "4"
Comp ""
Comment1 "Rev1: Floating all DPAD1 C&S pins as required by datasheet."
Comment2 "Rev2: Mechanical update to board only."
Comment3 "Rev3: Fixing footprint for linear regulator chips. Updating F1 fuse for higher A^2s rating."
Comment4 "Rev4: Resistors now 1206 & 0805. Oscillation fix: R11, R13 to 2.7k, added C19, C20."
$EndDescr
$Comp
L HighImpedanceBuffer:PJ-102AH J1
U 1 1 5D698F4A
P 850 950
F 0 "J1" H 907 1267 50  0000 C CNN
F 1 "PJ-102AH" H 907 1176 50  0000 C CNN
F 2 "HighImpedanceBuffer:PJ-102AH" H 900 910 50  0001 C CNN
F 3 "~" H 900 910 50  0001 C CNN
	1    850  950 
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:GND #PWR01
U 1 1 5D69ACE8
P 1200 1300
F 0 "#PWR01" H 1200 1050 50  0001 C CNN
F 1 "GND" H 1205 1127 50  0000 C CNN
F 2 "" H 1200 1300 50  0001 C CNN
F 3 "" H 1200 1300 50  0001 C CNN
	1    1200 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 1050 1200 1050
Wire Wire Line
	1200 1050 1200 1250
Wire Wire Line
	1150 850  1400 850 
$Comp
L HighImpedanceBuffer:SMBJ36CA D3
U 1 1 5D69B9A1
P 1750 1050
F 0 "D3" V 1704 1129 50  0000 L CNN
F 1 "SMBJ36CA" V 1795 1129 50  0000 L CNN
F 2 "HighImpedanceBuffer:D_SMB_Bidir" H 1750 1050 50  0001 C CNN
F 3 "~" H 1750 1050 50  0001 C CNN
	1    1750 1050
	0    1    1    0   
$EndComp
Wire Wire Line
	1200 1250 1750 1250
Wire Wire Line
	1750 1250 1750 1200
Connection ~ 1200 1250
Wire Wire Line
	1200 1250 1200 1300
Wire Wire Line
	1700 850  1750 850 
Wire Wire Line
	1750 850  1750 900 
$Comp
L HighImpedanceBuffer:NRVTSA4100E D5
U 1 1 5D69CCA4
P 2600 850
F 0 "D5" H 2600 634 50  0000 C CNN
F 1 "NRVTSA4100E" H 2600 725 50  0000 C CNN
F 2 "HighImpedanceBuffer:D_SMA" H 2600 850 50  0001 C CNN
F 3 "~" H 2600 850 50  0001 C CNN
	1    2600 850 
	-1   0    0    1   
$EndComp
Wire Wire Line
	1750 850  2250 850 
Connection ~ 1750 850 
$Comp
L HighImpedanceBuffer:CP C5
U 1 1 5D69E888
P 2850 1100
F 0 "C5" H 2968 1146 50  0000 L CNN
F 1 "470u" H 2968 1055 50  0000 L CNN
F 2 "HighImpedanceBuffer:TG_J16" H 2888 950 50  0001 C CNN
F 3 "~" H 2850 1100 50  0001 C CNN
	1    2850 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 850  2850 850 
Wire Wire Line
	2850 850  2850 950 
$Comp
L HighImpedanceBuffer:CP C6
U 1 1 5D69F318
P 2850 1600
F 0 "C6" H 2968 1646 50  0000 L CNN
F 1 "470u" H 2968 1555 50  0000 L CNN
F 2 "HighImpedanceBuffer:TG_J16" H 2888 1450 50  0001 C CNN
F 3 "~" H 2850 1600 50  0001 C CNN
	1    2850 1600
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:GND #PWR06
U 1 1 5D69FDE2
P 2800 1350
F 0 "#PWR06" H 2800 1100 50  0001 C CNN
F 1 "GND" H 2805 1177 50  0000 C CNN
F 2 "" H 2800 1350 50  0001 C CNN
F 3 "" H 2800 1350 50  0001 C CNN
	1    2800 1350
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:NRVTSA4100E D4
U 1 1 5D6A0482
P 2550 1900
F 0 "D4" H 2550 2116 50  0000 C CNN
F 1 "NRVTSA4100E" H 2550 2025 50  0000 C CNN
F 2 "HighImpedanceBuffer:D_SMA" H 2550 1900 50  0001 C CNN
F 3 "~" H 2550 1900 50  0001 C CNN
	1    2550 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 1750 2850 1900
Wire Wire Line
	2850 1900 2700 1900
Wire Wire Line
	2400 1900 2250 1900
Wire Wire Line
	2250 1900 2250 850 
Connection ~ 2250 850 
Wire Wire Line
	2250 850  2450 850 
$Comp
L HighImpedanceBuffer:R R5
U 1 1 5D6A15B9
P 3300 850
F 0 "R5" V 3200 850 50  0000 C CNN
F 1 "2.2" V 3300 850 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 3230 850 50  0001 C CNN
F 3 "~" H 3300 850 50  0001 C CNN
	1    3300 850 
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:R R6
U 1 1 5D6A1CB8
P 3300 1900
F 0 "R6" V 3200 1900 50  0000 C CNN
F 1 "2.2" V 3300 1900 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 3230 1900 50  0001 C CNN
F 3 "~" H 3300 1900 50  0001 C CNN
	1    3300 1900
	0    1    1    0   
$EndComp
Wire Wire Line
	3150 850  2850 850 
Connection ~ 2850 850 
Wire Wire Line
	3150 1900 2850 1900
Connection ~ 2850 1900
$Comp
L HighImpedanceBuffer:C C9
U 1 1 5D6A2A58
P 3550 1100
F 0 "C9" H 3665 1146 50  0000 L CNN
F 1 "10u" H 3665 1055 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_2220" H 3588 950 50  0001 C CNN
F 3 "~" H 3550 1100 50  0001 C CNN
	1    3550 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 850  3550 850 
Wire Wire Line
	3550 850  3550 950 
$Comp
L HighImpedanceBuffer:C C10
U 1 1 5D6A3FB2
P 3550 1600
F 0 "C10" H 3665 1646 50  0000 L CNN
F 1 "10u" H 3665 1555 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_2220" H 3588 1450 50  0001 C CNN
F 3 "~" H 3550 1600 50  0001 C CNN
	1    3550 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 1900 3550 1900
Wire Wire Line
	3550 1900 3550 1750
$Comp
L HighImpedanceBuffer:TPS7A4901DGNR U1
U 1 1 5D6A6549
P 1900 3350
F 0 "U1" H 1900 3700 50  0000 C CNN
F 1 "TPS7A4901DGNR" H 1900 3100 50  0000 C CNN
F 2 "HighImpedanceBuffer:MSOP-8-EP" H 1900 3350 50  0001 C CNN
F 3 "" H 1900 3350 50  0001 C CNN
	1    1900 3350
	1    0    0    -1  
$EndComp
Text Label 3950 850  2    50   ~ 0
PreReg+
Wire Wire Line
	3550 850  3950 850 
Connection ~ 3550 850 
Wire Wire Line
	3550 1900 3950 1900
Connection ~ 3550 1900
Text Label 3950 1900 2    50   ~ 0
PreReg-
$Comp
L HighImpedanceBuffer:C C7
U 1 1 5D6A8C06
P 2950 3600
F 0 "C7" H 3065 3646 50  0000 L CNN
F 1 "10n_NP0" H 3065 3555 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_0603_HandSoldering" H 2988 3450 50  0001 C CNN
F 3 "~" H 2950 3600 50  0001 C CNN
	1    2950 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 3500 2800 3500
Text Label 2800 3500 2    50   ~ 0
PreReg+
Wire Wire Line
	2450 3400 2950 3400
Wire Wire Line
	2950 3400 2950 3450
$Comp
L HighImpedanceBuffer:GND #PWR07
U 1 1 5D6AA422
P 2950 3800
F 0 "#PWR07" H 2950 3550 50  0001 C CNN
F 1 "GND" H 2955 3627 50  0000 C CNN
F 2 "" H 2950 3800 50  0001 C CNN
F 3 "" H 2950 3800 50  0001 C CNN
	1    2950 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 3750 2950 3800
Wire Wire Line
	2450 3200 2800 3200
Text Label 2800 3200 2    50   ~ 0
PreReg+
$Comp
L HighImpedanceBuffer:GND #PWR02
U 1 1 5D6ABE5D
P 1300 3750
F 0 "#PWR02" H 1300 3500 50  0001 C CNN
F 1 "GND" H 1305 3577 50  0000 C CNN
F 2 "" H 1300 3750 50  0001 C CNN
F 3 "" H 1300 3750 50  0001 C CNN
	1    1300 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 3500 1300 3500
Wire Wire Line
	1300 3500 1300 3700
$Comp
L HighImpedanceBuffer:R R3
U 1 1 5D6ADF9E
P 900 3150
F 0 "R3" V 800 3150 50  0000 C CNN
F 1 "88.7k" V 900 3150 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0603_HandSoldering" V 830 3150 50  0001 C CNN
F 3 "~" H 900 3150 50  0001 C CNN
	1    900  3150
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:C C1
U 1 1 5D6AECB9
P 900 2900
F 0 "C1" V 648 2900 50  0000 C CNN
F 1 "10n_NP0" V 739 2900 50  0000 C CNN
F 2 "HighImpedanceBuffer:C_0603_HandSoldering" H 938 2750 50  0001 C CNN
F 3 "~" H 900 2900 50  0001 C CNN
	1    900  2900
	0    1    1    0   
$EndComp
Wire Wire Line
	1350 3200 1250 3200
Wire Wire Line
	1250 3200 1250 2900
Wire Wire Line
	1250 2900 1100 2900
Wire Wire Line
	1050 3150 1100 3150
Wire Wire Line
	1100 3150 1100 2900
Connection ~ 1100 2900
Wire Wire Line
	1100 2900 1050 2900
Wire Wire Line
	750  2900 700  2900
Wire Wire Line
	700  2900 700  3150
Wire Wire Line
	700  3300 1350 3300
Wire Wire Line
	750  3150 700  3150
Connection ~ 700  3150
Wire Wire Line
	700  3150 700  3300
$Comp
L HighImpedanceBuffer:R R1
U 1 1 5D6B1FB0
P 700 3500
F 0 "R1" H 770 3546 50  0000 L CNN
F 1 "6.2k" H 770 3455 50  0000 L CNN
F 2 "HighImpedanceBuffer:R_0603_HandSoldering" V 630 3500 50  0001 C CNN
F 3 "~" H 700 3500 50  0001 C CNN
	1    700  3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	700  3300 700  3350
Connection ~ 700  3300
Wire Wire Line
	700  3650 700  3700
Wire Wire Line
	700  3700 1300 3700
Connection ~ 1300 3700
Wire Wire Line
	1300 3700 1300 3750
$Comp
L HighImpedanceBuffer:C C3
U 1 1 5D6B764A
P 1650 2900
F 0 "C3" V 1398 2900 50  0000 C CNN
F 1 "10u" V 1489 2900 50  0000 C CNN
F 2 "HighImpedanceBuffer:C_2220" H 1688 2750 50  0001 C CNN
F 3 "~" H 1650 2900 50  0001 C CNN
	1    1650 2900
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:GND #PWR04
U 1 1 5D6B8A9C
P 2550 2900
F 0 "#PWR04" H 2550 2650 50  0001 C CNN
F 1 "GND" V 2555 2772 50  0000 R CNN
F 2 "" H 2550 2900 50  0001 C CNN
F 3 "" H 2550 2900 50  0001 C CNN
	1    2550 2900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2550 2900 2500 2900
Wire Wire Line
	1500 2900 1250 2900
Connection ~ 1250 2900
Text Label 1300 2900 0    50   ~ 0
18V+
$Comp
L HighImpedanceBuffer:C C8
U 1 1 5D6BFD19
P 2950 5600
F 0 "C8" H 3065 5646 50  0000 L CNN
F 1 "10n_NP0" H 3065 5555 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_0603_HandSoldering" H 2988 5450 50  0001 C CNN
F 3 "~" H 2950 5600 50  0001 C CNN
	1    2950 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 5500 2800 5500
Text Label 2800 5500 2    50   ~ 0
PreReg-
Wire Wire Line
	2450 5400 2950 5400
Wire Wire Line
	2950 5400 2950 5450
$Comp
L HighImpedanceBuffer:GND #PWR08
U 1 1 5D6BFD27
P 2950 5800
F 0 "#PWR08" H 2950 5550 50  0001 C CNN
F 1 "GND" H 2955 5627 50  0000 C CNN
F 2 "" H 2950 5800 50  0001 C CNN
F 3 "" H 2950 5800 50  0001 C CNN
	1    2950 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 5750 2950 5800
Wire Wire Line
	2450 5200 2800 5200
Text Label 2800 5200 2    50   ~ 0
PreReg-
$Comp
L HighImpedanceBuffer:GND #PWR03
U 1 1 5D6BFD40
P 1300 5750
F 0 "#PWR03" H 1300 5500 50  0001 C CNN
F 1 "GND" H 1305 5577 50  0000 C CNN
F 2 "" H 1300 5750 50  0001 C CNN
F 3 "" H 1300 5750 50  0001 C CNN
	1    1300 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 5500 1300 5500
Wire Wire Line
	1300 5500 1300 5700
$Comp
L HighImpedanceBuffer:R R4
U 1 1 5D6BFD4C
P 900 5150
F 0 "R4" V 800 5150 50  0000 C CNN
F 1 "88.7k" V 900 5150 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0603_HandSoldering" V 830 5150 50  0001 C CNN
F 3 "~" H 900 5150 50  0001 C CNN
	1    900  5150
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:C C2
U 1 1 5D6BFD56
P 900 4900
F 0 "C2" V 648 4900 50  0000 C CNN
F 1 "10n_NP0" V 739 4900 50  0000 C CNN
F 2 "HighImpedanceBuffer:C_0603_HandSoldering" H 938 4750 50  0001 C CNN
F 3 "~" H 900 4900 50  0001 C CNN
	1    900  4900
	0    1    1    0   
$EndComp
Wire Wire Line
	1350 5200 1250 5200
Wire Wire Line
	1250 5200 1250 4900
Wire Wire Line
	1250 4900 1100 4900
Wire Wire Line
	1050 5150 1100 5150
Wire Wire Line
	1100 5150 1100 4900
Connection ~ 1100 4900
Wire Wire Line
	1100 4900 1050 4900
Wire Wire Line
	750  4900 700  4900
Wire Wire Line
	700  4900 700  5150
Wire Wire Line
	700  5300 1350 5300
Wire Wire Line
	750  5150 700  5150
Connection ~ 700  5150
Wire Wire Line
	700  5150 700  5300
$Comp
L HighImpedanceBuffer:R R2
U 1 1 5D6BFD6D
P 700 5500
F 0 "R2" H 770 5546 50  0000 L CNN
F 1 "6.2k" H 770 5455 50  0000 L CNN
F 2 "HighImpedanceBuffer:R_0603_HandSoldering" V 630 5500 50  0001 C CNN
F 3 "~" H 700 5500 50  0001 C CNN
	1    700  5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	700  5300 700  5350
Connection ~ 700  5300
Wire Wire Line
	700  5650 700  5700
Wire Wire Line
	700  5700 1300 5700
Connection ~ 1300 5700
Wire Wire Line
	1300 5700 1300 5750
$Comp
L HighImpedanceBuffer:C C4
U 1 1 5D6BFD7D
P 1650 4900
F 0 "C4" V 1398 4900 50  0000 C CNN
F 1 "10u" V 1489 4900 50  0000 C CNN
F 2 "HighImpedanceBuffer:C_2220" H 1688 4750 50  0001 C CNN
F 3 "~" H 1650 4900 50  0001 C CNN
	1    1650 4900
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:GND #PWR05
U 1 1 5D6BFD87
P 2550 4900
F 0 "#PWR05" H 2550 4650 50  0001 C CNN
F 1 "GND" V 2555 4772 50  0000 R CNN
F 2 "" H 2550 4900 50  0001 C CNN
F 3 "" H 2550 4900 50  0001 C CNN
	1    2550 4900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2550 4900 2500 4900
Wire Wire Line
	1500 4900 1250 4900
Connection ~ 1250 4900
Text Label 1300 4900 0    50   ~ 0
18V-
Text Notes 650  1850 0    50   ~ 0
20VAC 200mA Input,\n2.1mm ID, 5.5mm OD,\nTriad WAU20-200\nor similar
$Comp
L HighImpedanceBuffer:OPA192IDR U3
U 1 1 5D6E0460
P 7600 1450
F 0 "U3" H 7600 1700 50  0000 C CNN
F 1 "OPA192IDR" H 7600 1200 50  0000 C CNN
F 2 "HighImpedanceBuffer:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 7600 1450 50  0001 C CNN
F 3 "" H 7600 1450 50  0001 C CNN
	1    7600 1450
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:SMBJ18A D1
U 1 1 5D6E0EBC
P 1650 2500
F 0 "D1" H 1650 2716 50  0000 C CNN
F 1 "SMBJ18A" H 1650 2625 50  0000 C CNN
F 2 "HighImpedanceBuffer:D_SMB" H 1650 2500 50  0001 C CNN
F 3 "~" H 1650 2500 50  0001 C CNN
	1    1650 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 2500 1250 2500
Wire Wire Line
	1250 2500 1250 2900
Wire Wire Line
	1800 2500 1900 2500
Wire Wire Line
	1900 2500 1900 2900
Connection ~ 1900 2900
Wire Wire Line
	1900 2900 1800 2900
$Comp
L HighImpedanceBuffer:SMBJ18A D2
U 1 1 5D6F6605
P 1650 4500
F 0 "D2" H 1650 4716 50  0000 C CNN
F 1 "SMBJ18A" H 1650 4625 50  0000 C CNN
F 2 "HighImpedanceBuffer:D_SMB" H 1650 4500 50  0001 C CNN
F 3 "~" H 1650 4500 50  0001 C CNN
	1    1650 4500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1500 4500 1250 4500
Wire Wire Line
	1250 4500 1250 4900
Wire Wire Line
	1800 4500 1900 4500
Wire Wire Line
	1900 4500 1900 4900
Connection ~ 1900 4900
Wire Wire Line
	1900 4900 1800 4900
$Comp
L HighImpedanceBuffer:C C13
U 1 1 5D6FE018
P 7050 1900
F 0 "C13" H 7165 1946 50  0000 L CNN
F 1 "100n" H 7165 1855 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_0805_HandSoldering" H 7088 1750 50  0001 C CNN
F 3 "~" H 7050 1900 50  0001 C CNN
	1    7050 1900
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:C C11
U 1 1 5D6FE8EF
P 6650 1900
F 0 "C11" H 6765 1946 50  0000 L CNN
F 1 "1u" H 6765 1855 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_1206" H 6688 1750 50  0001 C CNN
F 3 "~" H 6650 1900 50  0001 C CNN
	1    6650 1900
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:GND #PWR013
U 1 1 5D6FEF85
P 6650 2150
F 0 "#PWR013" H 6650 1900 50  0001 C CNN
F 1 "GND" H 6655 1977 50  0000 C CNN
F 2 "" H 6650 2150 50  0001 C CNN
F 3 "" H 6650 2150 50  0001 C CNN
	1    6650 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 2050 6650 2100
Wire Wire Line
	6650 2100 7050 2100
Wire Wire Line
	7050 2100 7050 2050
Connection ~ 6650 2100
Wire Wire Line
	6650 2100 6650 2150
Wire Wire Line
	7150 1600 7050 1600
Wire Wire Line
	6650 1600 6650 1750
Wire Wire Line
	7050 1750 7050 1600
Connection ~ 7050 1600
Wire Wire Line
	7050 1600 6650 1600
Text Label 6800 1600 0    50   ~ 0
18V-
$Comp
L HighImpedanceBuffer:C C15
U 1 1 5D70B5EC
P 8250 1200
F 0 "C15" H 8365 1246 50  0000 L CNN
F 1 "100n" H 8365 1155 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_0805_HandSoldering" H 8288 1050 50  0001 C CNN
F 3 "~" H 8250 1200 50  0001 C CNN
	1    8250 1200
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:C C17
U 1 1 5D70BD73
P 8700 1200
F 0 "C17" H 8815 1246 50  0000 L CNN
F 1 "1u" H 8815 1155 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_1206" H 8738 1050 50  0001 C CNN
F 3 "~" H 8700 1200 50  0001 C CNN
	1    8700 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 1400 8250 1400
Wire Wire Line
	8700 1400 8700 1350
Wire Wire Line
	8250 1350 8250 1400
Connection ~ 8250 1400
Wire Wire Line
	8250 1400 8700 1400
$Comp
L HighImpedanceBuffer:GND #PWR017
U 1 1 5D712806
P 8250 950
F 0 "#PWR017" H 8250 700 50  0001 C CNN
F 1 "GND" H 8255 777 50  0000 C CNN
F 2 "" H 8250 950 50  0001 C CNN
F 3 "" H 8250 950 50  0001 C CNN
	1    8250 950 
	-1   0    0    1   
$EndComp
Wire Wire Line
	8250 950  8250 1000
Wire Wire Line
	8250 1000 8700 1000
Wire Wire Line
	8700 1000 8700 1050
Connection ~ 8250 1000
Wire Wire Line
	8250 1000 8250 1050
Text Label 8400 1400 0    50   ~ 0
18V+
$Comp
L HighImpedanceBuffer:R R9
U 1 1 5D71A049
P 6450 1500
F 0 "R9" V 6350 1500 50  0000 C CNN
F 1 "1k" V 6450 1500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0805_HandSoldering" V 6380 1500 50  0001 C CNN
F 3 "~" H 6450 1500 50  0001 C CNN
	1    6450 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	6600 1500 7150 1500
$Comp
L HighImpedanceBuffer:DPAD1 D6
U 1 1 5D71E3AB
P 5600 1850
F 0 "D6" H 5600 1650 50  0000 C CNN
F 1 "DPAD1" H 5600 2050 50  0000 C CNN
F 2 "HighImpedanceBuffer:TO-78-5" H 5600 1850 50  0001 C CNN
F 3 "" H 5600 1850 50  0001 C CNN
	1    5600 1850
	-1   0    0    1   
$EndComp
Wire Wire Line
	6050 1850 6200 1850
Wire Wire Line
	6200 1850 6200 1500
Wire Wire Line
	6200 1500 6300 1500
Wire Wire Line
	6200 1500 5100 1500
Wire Wire Line
	5100 1500 5100 1750
Wire Wire Line
	5100 1750 5150 1750
Connection ~ 6200 1500
Text Label 6300 1950 2    50   ~ 0
18V+
Wire Wire Line
	6300 1950 6050 1950
Text Label 4900 1950 0    50   ~ 0
18V-
Wire Wire Line
	4900 1950 5150 1950
$Comp
L HighImpedanceBuffer:R R7
U 1 1 5D73C0E5
P 4900 1500
F 0 "R7" V 4800 1500 50  0000 C CNN
F 1 "10k" V 4900 1500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 4830 1500 50  0001 C CNN
F 3 "~" H 4900 1500 50  0001 C CNN
	1    4900 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 1500 5100 1500
Connection ~ 5100 1500
Wire Wire Line
	4750 1500 4600 1500
Text Label 4600 1500 0    50   ~ 0
IN1
$Comp
L HighImpedanceBuffer:R R12
U 1 1 5D748B88
P 6900 1400
F 0 "R12" V 6800 1400 50  0000 C CNN
F 1 "1k" V 6900 1400 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0805_HandSoldering" V 6830 1400 50  0001 C CNN
F 3 "~" H 6900 1400 50  0001 C CNN
	1    6900 1400
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 1400 7150 1400
$Comp
L HighImpedanceBuffer:DPAD1 D8
U 1 1 5D74D414
P 6150 900
F 0 "D8" H 6150 1100 50  0000 C CNN
F 1 "DPAD1" H 6150 700 50  0000 C CNN
F 2 "HighImpedanceBuffer:TO-78-5" H 6150 900 50  0001 C CNN
F 3 "" H 6150 900 50  0001 C CNN
	1    6150 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 1000 6700 1000
Wire Wire Line
	6700 1000 6700 1250
Wire Wire Line
	6700 1400 6750 1400
Wire Wire Line
	5700 900  5550 900 
Wire Wire Line
	5550 900  5550 1250
Wire Wire Line
	5550 1250 6700 1250
Connection ~ 6700 1250
Wire Wire Line
	6700 1250 6700 1400
$Comp
L HighImpedanceBuffer:R R11
U 1 1 5D7663B3
P 7150 1000
F 0 "R11" V 7050 1000 50  0000 C CNN
F 1 "2.7k" V 7150 1000 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 7080 1000 50  0001 C CNN
F 3 "~" H 7150 1000 50  0001 C CNN
	1    7150 1000
	0    1    1    0   
$EndComp
Wire Wire Line
	7000 1000 6950 1000
Connection ~ 6700 1000
Wire Wire Line
	7300 1000 7350 1000
Text Label 7600 1000 2    50   ~ 0
OUT1
Text Label 5450 800  0    50   ~ 0
18V+
Wire Wire Line
	5450 800  5700 800 
Text Label 6850 800  2    50   ~ 0
18V-
Wire Wire Line
	6850 800  6600 800 
$Comp
L HighImpedanceBuffer:R R15
U 1 1 5D77F9F7
P 9050 1500
F 0 "R15" V 8950 1500 50  0000 C CNN
F 1 "20" V 9050 1500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0805_HandSoldering" V 8980 1500 50  0001 C CNN
F 3 "~" H 9050 1500 50  0001 C CNN
	1    9050 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	8050 1500 8900 1500
$Comp
L HighImpedanceBuffer:CMDD6001 D10
U 1 1 5D785414
P 9300 1250
F 0 "D10" V 9254 1329 50  0000 L CNN
F 1 "CMDD6001" V 9345 1329 50  0000 L CNN
F 2 "HighImpedanceBuffer:D_SOD-323_HandSoldering" H 9300 1250 50  0001 C CNN
F 3 "~" H 9300 1250 50  0001 C CNN
	1    9300 1250
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:CMDD6001 D11
U 1 1 5D78671E
P 9300 1750
F 0 "D11" V 9254 1829 50  0000 L CNN
F 1 "CMDD6001" V 9345 1829 50  0000 L CNN
F 2 "HighImpedanceBuffer:D_SOD-323_HandSoldering" H 9300 1750 50  0001 C CNN
F 3 "~" H 9300 1750 50  0001 C CNN
	1    9300 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	9300 1600 9300 1500
Wire Wire Line
	9300 1500 9200 1500
Wire Wire Line
	9300 1500 9300 1400
Connection ~ 9300 1500
$Comp
L HighImpedanceBuffer:R R17
U 1 1 5D791382
P 9550 1500
F 0 "R17" V 9450 1500 50  0000 C CNN
F 1 "100" V 9550 1500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 9480 1500 50  0001 C CNN
F 3 "~" H 9550 1500 50  0001 C CNN
	1    9550 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	9400 1500 9300 1500
$Comp
L HighImpedanceBuffer:MMBZ27VAL D14
U 1 1 5D796C86
P 10100 1850
F 0 "D14" H 10100 2100 50  0000 C CNN
F 1 "MMBZ27VAL" H 10000 1600 50  0000 C CNN
F 2 "HighImpedanceBuffer:SOT-23" H 10000 2100 50  0001 C CNN
F 3 "https://en.wikipedia.org/wiki/Zener_diode" H 10000 2100 50  0001 C CNN
	1    10100 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9700 1500 9900 1500
Wire Wire Line
	9900 1500 9900 1750
Wire Wire Line
	9900 1750 9950 1750
Wire Wire Line
	9950 1950 9900 1950
$Comp
L HighImpedanceBuffer:GND #PWR019
U 1 1 5D7A3E3C
P 9900 2100
F 0 "#PWR019" H 9900 1850 50  0001 C CNN
F 1 "GND" H 9905 1927 50  0000 C CNN
F 2 "" H 9900 2100 50  0001 C CNN
F 3 "" H 9900 2100 50  0001 C CNN
	1    9900 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 1950 9900 2100
Text Label 9900 1500 2    50   ~ 0
OUT1
Text Label 9050 2000 0    50   ~ 0
18V-
Wire Wire Line
	9050 2000 9300 2000
Wire Wire Line
	9300 2000 9300 1900
Text Label 9100 1000 0    50   ~ 0
18V+
Wire Wire Line
	9100 1000 9300 1000
Wire Wire Line
	9300 1000 9300 1100
$Comp
L HighImpedanceBuffer:OPA192IDR U4
U 1 1 5D7BBE6C
P 7600 3450
F 0 "U4" H 7600 3700 50  0000 C CNN
F 1 "OPA192IDR" H 7600 3200 50  0000 C CNN
F 2 "HighImpedanceBuffer:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 7600 3450 50  0001 C CNN
F 3 "" H 7600 3450 50  0001 C CNN
	1    7600 3450
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:C C14
U 1 1 5D7BBE76
P 7050 3900
F 0 "C14" H 7165 3946 50  0000 L CNN
F 1 "100n" H 7165 3855 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_0805_HandSoldering" H 7088 3750 50  0001 C CNN
F 3 "~" H 7050 3900 50  0001 C CNN
	1    7050 3900
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:C C12
U 1 1 5D7BBE80
P 6650 3900
F 0 "C12" H 6765 3946 50  0000 L CNN
F 1 "1u" H 6765 3855 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_1206" H 6688 3750 50  0001 C CNN
F 3 "~" H 6650 3900 50  0001 C CNN
	1    6650 3900
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:GND #PWR014
U 1 1 5D7BBE8A
P 6650 4150
F 0 "#PWR014" H 6650 3900 50  0001 C CNN
F 1 "GND" H 6655 3977 50  0000 C CNN
F 2 "" H 6650 4150 50  0001 C CNN
F 3 "" H 6650 4150 50  0001 C CNN
	1    6650 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4050 6650 4100
Wire Wire Line
	6650 4100 7050 4100
Wire Wire Line
	7050 4100 7050 4050
Connection ~ 6650 4100
Wire Wire Line
	6650 4100 6650 4150
Wire Wire Line
	7150 3600 7050 3600
Wire Wire Line
	6650 3600 6650 3750
Wire Wire Line
	7050 3750 7050 3600
Connection ~ 7050 3600
Wire Wire Line
	7050 3600 6650 3600
Text Label 6800 3600 0    50   ~ 0
18V-
$Comp
L HighImpedanceBuffer:C C16
U 1 1 5D7BBE9F
P 8250 3200
F 0 "C16" H 8365 3246 50  0000 L CNN
F 1 "100n" H 8365 3155 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_0805_HandSoldering" H 8288 3050 50  0001 C CNN
F 3 "~" H 8250 3200 50  0001 C CNN
	1    8250 3200
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:C C18
U 1 1 5D7BBEA9
P 8700 3200
F 0 "C18" H 8815 3246 50  0000 L CNN
F 1 "1u" H 8815 3155 50  0000 L CNN
F 2 "HighImpedanceBuffer:C_1206" H 8738 3050 50  0001 C CNN
F 3 "~" H 8700 3200 50  0001 C CNN
	1    8700 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 3400 8250 3400
Wire Wire Line
	8700 3400 8700 3350
Wire Wire Line
	8250 3350 8250 3400
Connection ~ 8250 3400
Wire Wire Line
	8250 3400 8700 3400
$Comp
L HighImpedanceBuffer:GND #PWR018
U 1 1 5D7BBEB8
P 8250 2950
F 0 "#PWR018" H 8250 2700 50  0001 C CNN
F 1 "GND" H 8255 2777 50  0000 C CNN
F 2 "" H 8250 2950 50  0001 C CNN
F 3 "" H 8250 2950 50  0001 C CNN
	1    8250 2950
	-1   0    0    1   
$EndComp
Wire Wire Line
	8250 2950 8250 3000
Wire Wire Line
	8250 3000 8700 3000
Wire Wire Line
	8700 3000 8700 3050
Connection ~ 8250 3000
Wire Wire Line
	8250 3000 8250 3050
Text Label 8400 3400 0    50   ~ 0
18V+
$Comp
L HighImpedanceBuffer:R R10
U 1 1 5D7BBEC8
P 6450 3500
F 0 "R10" V 6350 3500 50  0000 C CNN
F 1 "1k" V 6450 3500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0805_HandSoldering" V 6380 3500 50  0001 C CNN
F 3 "~" H 6450 3500 50  0001 C CNN
	1    6450 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	6600 3500 7150 3500
$Comp
L HighImpedanceBuffer:DPAD1 D7
U 1 1 5D7BBED3
P 5600 3850
F 0 "D7" H 5600 3650 50  0000 C CNN
F 1 "DPAD1" H 5600 4050 50  0000 C CNN
F 2 "HighImpedanceBuffer:TO-78-5" H 5600 3850 50  0001 C CNN
F 3 "" H 5600 3850 50  0001 C CNN
	1    5600 3850
	-1   0    0    1   
$EndComp
Wire Wire Line
	6050 3850 6200 3850
Wire Wire Line
	6200 3850 6200 3500
Wire Wire Line
	6200 3500 6300 3500
Wire Wire Line
	6200 3500 5100 3500
Wire Wire Line
	5100 3500 5100 3750
Wire Wire Line
	5100 3750 5150 3750
Connection ~ 6200 3500
Text Label 6300 3950 2    50   ~ 0
18V+
Wire Wire Line
	6300 3950 6050 3950
Text Label 4900 3950 0    50   ~ 0
18V-
Wire Wire Line
	4900 3950 5150 3950
$Comp
L HighImpedanceBuffer:R R8
U 1 1 5D7BBEF5
P 4900 3500
F 0 "R8" V 4800 3500 50  0000 C CNN
F 1 "10k" V 4900 3500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 4830 3500 50  0001 C CNN
F 3 "~" H 4900 3500 50  0001 C CNN
	1    4900 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 3500 5100 3500
Connection ~ 5100 3500
Wire Wire Line
	4750 3500 4600 3500
Text Label 4600 3500 0    50   ~ 0
IN2
$Comp
L HighImpedanceBuffer:R R14
U 1 1 5D7BBF03
P 6900 3400
F 0 "R14" V 6800 3400 50  0000 C CNN
F 1 "1k" V 6900 3400 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0805_HandSoldering" V 6830 3400 50  0001 C CNN
F 3 "~" H 6900 3400 50  0001 C CNN
	1    6900 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 3400 7150 3400
$Comp
L HighImpedanceBuffer:DPAD1 D9
U 1 1 5D7BBF0E
P 6150 2900
F 0 "D9" H 6150 3100 50  0000 C CNN
F 1 "DPAD1" H 6150 2700 50  0000 C CNN
F 2 "HighImpedanceBuffer:TO-78-5" H 6150 2900 50  0001 C CNN
F 3 "" H 6150 2900 50  0001 C CNN
	1    6150 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 3000 6700 3000
Wire Wire Line
	6700 3000 6700 3250
Wire Wire Line
	6700 3400 6750 3400
Wire Wire Line
	5700 2900 5550 2900
Wire Wire Line
	5550 2900 5550 3250
Wire Wire Line
	5550 3250 6700 3250
Connection ~ 6700 3250
Wire Wire Line
	6700 3250 6700 3400
$Comp
L HighImpedanceBuffer:R R13
U 1 1 5D7BBF2D
P 7200 3000
F 0 "R13" V 7100 3000 50  0000 C CNN
F 1 "2.7k" V 7200 3000 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 7130 3000 50  0001 C CNN
F 3 "~" H 7200 3000 50  0001 C CNN
	1    7200 3000
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 3000 7000 3000
Connection ~ 6700 3000
Wire Wire Line
	7350 3000 7400 3000
Text Label 7650 3000 2    50   ~ 0
OUT2
Text Label 5450 2800 0    50   ~ 0
18V+
Wire Wire Line
	5450 2800 5700 2800
Text Label 6850 2800 2    50   ~ 0
18V-
Wire Wire Line
	6850 2800 6600 2800
$Comp
L HighImpedanceBuffer:R R16
U 1 1 5D7BBF3F
P 9050 3500
F 0 "R16" V 8950 3500 50  0000 C CNN
F 1 "20" V 9050 3500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_0805_HandSoldering" V 8980 3500 50  0001 C CNN
F 3 "~" H 9050 3500 50  0001 C CNN
	1    9050 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	8050 3500 8900 3500
$Comp
L HighImpedanceBuffer:CMDD6001 D12
U 1 1 5D7BBF4A
P 9300 3250
F 0 "D12" V 9254 3329 50  0000 L CNN
F 1 "CMDD6001" V 9345 3329 50  0000 L CNN
F 2 "HighImpedanceBuffer:D_SOD-323_HandSoldering" H 9300 3250 50  0001 C CNN
F 3 "~" H 9300 3250 50  0001 C CNN
	1    9300 3250
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:CMDD6001 D13
U 1 1 5D7BBF54
P 9300 3750
F 0 "D13" V 9254 3829 50  0000 L CNN
F 1 "CMDD6001" V 9345 3829 50  0000 L CNN
F 2 "HighImpedanceBuffer:D_SOD-323_HandSoldering" H 9300 3750 50  0001 C CNN
F 3 "~" H 9300 3750 50  0001 C CNN
	1    9300 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	9300 3600 9300 3500
Wire Wire Line
	9300 3500 9200 3500
Wire Wire Line
	9300 3500 9300 3400
Connection ~ 9300 3500
$Comp
L HighImpedanceBuffer:R R18
U 1 1 5D7BBF62
P 9550 3500
F 0 "R18" V 9450 3500 50  0000 C CNN
F 1 "100" V 9550 3500 50  0000 C CNN
F 2 "HighImpedanceBuffer:R_1206" V 9480 3500 50  0001 C CNN
F 3 "~" H 9550 3500 50  0001 C CNN
	1    9550 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	9400 3500 9300 3500
$Comp
L HighImpedanceBuffer:MMBZ27VAL D15
U 1 1 5D7BBF6D
P 10100 3850
F 0 "D15" H 10100 4100 50  0000 C CNN
F 1 "MMBZ27VAL" H 10000 3600 50  0000 C CNN
F 2 "HighImpedanceBuffer:SOT-23" H 10000 4100 50  0001 C CNN
F 3 "https://en.wikipedia.org/wiki/Zener_diode" H 10000 4100 50  0001 C CNN
	1    10100 3850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9700 3500 9900 3500
Wire Wire Line
	9900 3500 9900 3750
Wire Wire Line
	9900 3750 9950 3750
Wire Wire Line
	9950 3950 9900 3950
$Comp
L HighImpedanceBuffer:GND #PWR020
U 1 1 5D7BBF7B
P 9900 4100
F 0 "#PWR020" H 9900 3850 50  0001 C CNN
F 1 "GND" H 9905 3927 50  0000 C CNN
F 2 "" H 9900 4100 50  0001 C CNN
F 3 "" H 9900 4100 50  0001 C CNN
	1    9900 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9900 3950 9900 4100
Text Label 9900 3500 2    50   ~ 0
OUT2
Text Label 9050 4000 0    50   ~ 0
18V-
Wire Wire Line
	9050 4000 9300 4000
Wire Wire Line
	9300 4000 9300 3900
Text Label 9100 3000 0    50   ~ 0
18V+
Wire Wire Line
	9100 3000 9300 3000
Wire Wire Line
	9300 3000 9300 3100
$Comp
L HighImpedanceBuffer:1776540 J2
U 1 1 5D7D3505
P 7300 5250
F 0 "J2" H 7218 5667 50  0000 C CNN
F 1 "1776540" H 7218 5576 50  0000 C CNN
F 2 "HighImpedanceBuffer:1776540" H 7300 5250 50  0001 C CNN
F 3 "~" H 7300 5250 50  0001 C CNN
	1    7300 5250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7500 5050 7650 5050
$Comp
L HighImpedanceBuffer:GND #PWR015
U 1 1 5D7DFCD1
P 7700 5150
F 0 "#PWR015" H 7700 4900 50  0001 C CNN
F 1 "GND" V 7705 5022 50  0000 R CNN
F 2 "" H 7700 5150 50  0001 C CNN
F 3 "" H 7700 5150 50  0001 C CNN
	1    7700 5150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7700 5150 7500 5150
$Comp
L HighImpedanceBuffer:GND #PWR016
U 1 1 5D7EB6B8
P 7700 5450
F 0 "#PWR016" H 7700 5200 50  0001 C CNN
F 1 "GND" V 7705 5322 50  0000 R CNN
F 2 "" H 7700 5450 50  0001 C CNN
F 3 "" H 7700 5450 50  0001 C CNN
	1    7700 5450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7700 5450 7500 5450
Wire Wire Line
	7500 5250 7700 5250
Wire Wire Line
	7500 5350 7650 5350
Wire Wire Line
	7500 5550 7700 5550
Text Label 7650 5050 2    50   ~ 0
IN1
Text Label 7700 5250 2    50   ~ 0
OUT1
Text Label 7650 5350 2    50   ~ 0
IN2
Text Label 7700 5550 2    50   ~ 0
OUT2
Wire Wire Line
	2450 3100 2500 3100
Wire Wire Line
	2500 3100 2500 2900
Connection ~ 2500 2900
Wire Wire Line
	2500 2900 1900 2900
Wire Wire Line
	2450 5100 2500 5100
Wire Wire Line
	2500 5100 2500 4900
Connection ~ 2500 4900
Wire Wire Line
	2500 4900 1900 4900
Wire Wire Line
	2850 1250 2850 1350
Wire Wire Line
	2800 1350 2850 1350
Connection ~ 2850 1350
Wire Wire Line
	2850 1350 2850 1450
Wire Wire Line
	3550 1250 3550 1350
Wire Wire Line
	2850 1350 3550 1350
Connection ~ 3550 1350
Wire Wire Line
	3550 1350 3550 1450
Text Notes 7250 6050 0    50   ~ 0
1776540 receptacle mates with\n1778027 terminal block plug.\nAffix to board with 2 M2.2x6.5c\nthread forming screws,\nMcMaster 94997A125 or similar.\n
$Comp
L HighImpedanceBuffer:Screw_#2 S1
U 1 1 5D6B74A0
P 4300 4950
F 0 "S1" H 4399 5003 60  0000 L CNN
F 1 "Screw_#2" H 4399 4897 60  0000 L CNN
F 2 "HighImpedanceBuffer:Screw_#2" H 4300 4950 60  0001 C CNN
F 3 "" H 4300 4950 60  0001 C CNN
	1    4300 4950
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:Screw_#2 S3
U 1 1 5D6B838B
P 5050 4950
F 0 "S3" H 5149 5003 60  0000 L CNN
F 1 "Screw_#2" H 5149 4897 60  0000 L CNN
F 2 "HighImpedanceBuffer:Screw_#2" H 5050 4950 60  0001 C CNN
F 3 "" H 5050 4950 60  0001 C CNN
	1    5050 4950
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:Screw_#2 S5
U 1 1 5D6B851D
P 5750 4950
F 0 "S5" H 5849 5003 60  0000 L CNN
F 1 "Screw_#2" H 5849 4897 60  0000 L CNN
F 2 "HighImpedanceBuffer:Screw_#2" H 5750 4950 60  0001 C CNN
F 3 "" H 5750 4950 60  0001 C CNN
	1    5750 4950
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:Screw_#2 S2
U 1 1 5D6B88F0
P 4300 5350
F 0 "S2" H 4399 5403 60  0000 L CNN
F 1 "Screw_#2" H 4399 5297 60  0000 L CNN
F 2 "HighImpedanceBuffer:Screw_#2" H 4300 5350 60  0001 C CNN
F 3 "" H 4300 5350 60  0001 C CNN
	1    4300 5350
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:Screw_#2 S4
U 1 1 5D6B8B33
P 5050 5350
F 0 "S4" H 5149 5403 60  0000 L CNN
F 1 "Screw_#2" H 5149 5297 60  0000 L CNN
F 2 "HighImpedanceBuffer:Screw_#2" H 5050 5350 60  0001 C CNN
F 3 "" H 5050 5350 60  0001 C CNN
	1    5050 5350
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:Screw_#2 S6
U 1 1 5D6B8CFE
P 5750 5350
F 0 "S6" H 5849 5403 60  0000 L CNN
F 1 "Screw_#2" H 5849 5297 60  0000 L CNN
F 2 "HighImpedanceBuffer:Screw_#2" H 5750 5350 60  0001 C CNN
F 3 "" H 5750 5350 60  0001 C CNN
	1    5750 5350
	1    0    0    -1  
$EndComp
Text Notes 4450 5900 0    50   ~ 0
S1-S6 are #2-28 thread, 0.25" long shank,\n0.167" head diameter thread forming screws\nfor plastics, McMaster 90380A005 or similar.
$Comp
L HighImpedanceBuffer:TPS7A3001DGNR U2
U 1 1 5D6C8D8C
P 1900 5350
F 0 "U2" H 1900 5700 50  0000 C CNN
F 1 "TPS7A3001DGNR" H 1900 5100 50  0000 C CNN
F 2 "HighImpedanceBuffer:MSOP-8-EP" H 1900 5350 50  0001 C CNN
F 3 "" H 1900 5350 50  0001 C CNN
	1    1900 5350
	1    0    0    -1  
$EndComp
$Comp
L HighImpedanceBuffer:SF-2410FP0375T-2 F1
U 1 1 5D7E69DF
P 1550 850
F 0 "F1" V 1353 850 50  0000 C CNN
F 1 "SF-2410FP0375T-2" V 1444 850 50  0000 C CNN
F 2 "HighImpedanceBuffer:Fuse_2410" V 1480 850 50  0001 C CNN
F 3 "~" H 1550 850 50  0001 C CNN
	1    1550 850 
	0    1    1    0   
$EndComp
$Comp
L HighImpedanceBuffer:C C19
U 1 1 5D935838
P 7150 750
F 0 "C19" V 7000 750 50  0000 C CNN
F 1 "TBD" V 7050 900 50  0000 C CNN
F 2 "HighImpedanceBuffer:C_0603_HandSoldering" H 7188 600 50  0001 C CNN
F 3 "~" H 7150 750 50  0001 C CNN
	1    7150 750 
	0    1    1    0   
$EndComp
Wire Wire Line
	7000 750  6950 750 
Wire Wire Line
	6950 750  6950 1000
Connection ~ 6950 1000
Wire Wire Line
	6950 1000 6700 1000
Wire Wire Line
	7300 750  7350 750 
Wire Wire Line
	7350 750  7350 1000
Connection ~ 7350 1000
Wire Wire Line
	7350 1000 7600 1000
$Comp
L HighImpedanceBuffer:C C20
U 1 1 5D979632
P 7200 2750
F 0 "C20" V 7050 2750 50  0000 C CNN
F 1 "TBD" V 7100 2900 50  0000 C CNN
F 2 "HighImpedanceBuffer:C_0603_HandSoldering" H 7238 2600 50  0001 C CNN
F 3 "~" H 7200 2750 50  0001 C CNN
	1    7200 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 2750 7000 2750
Wire Wire Line
	7000 2750 7000 3000
Connection ~ 7000 3000
Wire Wire Line
	7000 3000 6700 3000
Wire Wire Line
	7350 2750 7400 2750
Wire Wire Line
	7400 2750 7400 3000
Connection ~ 7400 3000
Wire Wire Line
	7400 3000 7650 3000
$EndSCHEMATC
