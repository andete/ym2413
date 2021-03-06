EESchema Schematic File Version 2
LIBS:01jyd
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:audio-cache
EELAYER 24 0
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
Text Label 2500 2000 0    60   ~ 0
RO
Text Label 2500 2100 0    60   ~ 0
MO
$Comp
L R R40
U 1 1 52E3AC6B
P 2600 2300
F 0 "R40" H 2600 2350 40  0000 C CNN
F 1 "2k2/NC" H 2600 2250 40  0000 C CNN
F 2 "" H 2600 2300 60  0000 C CNN
F 3 "" H 2600 2300 60  0000 C CNN
	1    2600 2300
	0    -1   -1   0   
$EndComp
$Comp
L R R41
U 1 1 52E3AC84
P 2750 2300
F 0 "R41" H 2750 2350 40  0000 C CNN
F 1 "2k2/NC" H 2750 2250 40  0000 C CNN
F 2 "" H 2750 2300 60  0000 C CNN
F 3 "" H 2750 2300 60  0000 C CNN
	1    2750 2300
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR01
U 1 1 52E3ACE3
P 2600 2550
F 0 "#PWR01" H 2600 2550 30  0001 C CNN
F 1 "GND" H 2600 2480 30  0001 C CNN
F 2 "" H 2600 2550 60  0000 C CNN
F 3 "" H 2600 2550 60  0000 C CNN
	1    2600 2550
	1    0    0    -1  
$EndComp
$Comp
L C C18
U 1 1 52E3AD52
P 3050 2300
F 0 "C18" H 3050 2350 40  0000 C CNN
F 1 "2.2nF" H 3050 2250 40  0000 C CNN
F 2 "" H 3050 2300 60  0000 C CNN
F 3 "" H 3050 2300 60  0000 C CNN
	1    3050 2300
	0    -1   -1   0   
$EndComp
$Comp
L C C19
U 1 1 52E3AD71
P 3200 2300
F 0 "C19" H 3200 2350 40  0000 C CNN
F 1 "2.2nF" H 3200 2250 40  0000 C CNN
F 2 "" H 3200 2300 60  0000 C CNN
F 3 "" H 3200 2300 60  0000 C CNN
	1    3200 2300
	0    -1   -1   0   
$EndComp
$Comp
L R R44
U 1 1 52E3B3AF
P 3050 2000
F 0 "R44" H 3050 2050 40  0000 C CNN
F 1 "4k7/0" H 3050 1950 40  0000 C CNN
F 2 "" H 3050 2000 60  0000 C CNN
F 3 "" H 3050 2000 60  0000 C CNN
	1    3050 2000
	1    0    0    -1  
$EndComp
$Comp
L R R43
U 1 1 52E3B3BC
P 2900 2100
F 0 "R43" H 2900 2150 40  0000 C CNN
F 1 "4k7/0" H 2900 2050 40  0000 C CNN
F 2 "" H 2900 2100 60  0000 C CNN
F 3 "" H 2900 2100 60  0000 C CNN
	1    2900 2100
	1    0    0    -1  
$EndComp
Text Label 3250 2000 0    60   ~ 0
ROI
Text Label 3250 2100 0    60   ~ 0
MOI
$Comp
L LM386 U5
U 1 1 52F72FCC
P 5850 1500
F 0 "U5" H 6000 1700 60  0000 C CNN
F 1 "LM386" H 6000 1800 60  0000 C CNN
F 2 "" H 5850 1500 60  0000 C CNN
F 3 "" H 5850 1500 60  0000 C CNN
	1    5850 1500
	1    0    0    -1  
$EndComp
$Comp
L C C13
U 1 1 52F72FE5
P 4250 1900
F 0 "C13" H 4250 1950 40  0000 C CNN
F 1 "10uF" H 4250 1850 40  0000 C CNN
F 2 "" H 4250 1900 60  0000 C CNN
F 3 "" H 4250 1900 60  0000 C CNN
	1    4250 1900
	1    0    0    -1  
$EndComp
$Comp
L C C14
U 1 1 52F72FF2
P 4250 2150
F 0 "C14" H 4250 2200 40  0000 C CNN
F 1 "10uF" H 4250 2100 40  0000 C CNN
F 2 "" H 4250 2150 60  0000 C CNN
F 3 "" H 4250 2150 60  0000 C CNN
	1    4250 2150
	1    0    0    -1  
$EndComp
$Comp
L R R37
U 1 1 52F731EC
P 4450 2300
F 0 "R37" H 4450 2350 40  0000 C CNN
F 1 "10k" H 4450 2250 40  0000 C CNN
F 2 "" H 4450 2300 60  0000 C CNN
F 3 "" H 4450 2300 60  0000 C CNN
	1    4450 2300
	0    -1   -1   0   
$EndComp
$Comp
L R R39
U 1 1 52F731F9
P 4600 2300
F 0 "R39" H 4600 2350 40  0000 C CNN
F 1 "10k" H 4600 2250 40  0000 C CNN
F 2 "" H 4600 2300 60  0000 C CNN
F 3 "" H 4600 2300 60  0000 C CNN
	1    4600 2300
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR02
U 1 1 52F73217
P 4450 2500
F 0 "#PWR02" H 4450 2500 30  0001 C CNN
F 1 "GND" H 4450 2430 30  0001 C CNN
F 2 "" H 4450 2500 60  0000 C CNN
F 3 "" H 4450 2500 60  0000 C CNN
	1    4450 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 52F736E3
P 5350 2100
F 0 "#PWR03" H 5350 2100 30  0001 C CNN
F 1 "GND" H 5350 2030 30  0001 C CNN
F 2 "" H 5350 2100 60  0000 C CNN
F 3 "" H 5350 2100 60  0000 C CNN
	1    5350 2100
	1    0    0    -1  
$EndComp
$Comp
L C C20
U 1 1 52F73CC9
P 5900 2050
F 0 "C20" H 5900 2100 40  0000 C CNN
F 1 "1uF" H 5900 2000 40  0000 C CNN
F 2 "" H 5900 2050 60  0000 C CNN
F 3 "" H 5900 2050 60  0000 C CNN
	1    5900 2050
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR04
U 1 1 52F73EE0
P 5750 1050
F 0 "#PWR04" H 5750 1140 20  0001 C CNN
F 1 "+5V" H 5750 1140 30  0000 C CNN
F 2 "" H 5750 1050 60  0000 C CNN
F 3 "" H 5750 1050 60  0000 C CNN
	1    5750 1050
	1    0    0    -1  
$EndComp
$Comp
L C C22
U 1 1 52F7414F
P 6500 1500
F 0 "C22" H 6500 1550 40  0000 C CNN
F 1 "10uF" H 6500 1450 40  0000 C CNN
F 2 "" H 6500 1500 60  0000 C CNN
F 3 "" H 6500 1500 60  0000 C CNN
	1    6500 1500
	1    0    0    -1  
$EndComp
$Comp
L R R48
U 1 1 52F7426C
P 6600 1700
F 0 "R48" H 6600 1750 40  0000 C CNN
F 1 "100" H 6600 1650 40  0000 C CNN
F 2 "" H 6600 1700 60  0000 C CNN
F 3 "" H 6600 1700 60  0000 C CNN
	1    6600 1700
	0    -1   -1   0   
$EndComp
$Comp
L R R51
U 1 1 52F74285
P 6700 1500
F 0 "R51" H 6700 1550 40  0000 C CNN
F 1 "2k2" H 6700 1450 40  0000 C CNN
F 2 "" H 6700 1500 60  0000 C CNN
F 3 "" H 6700 1500 60  0000 C CNN
	1    6700 1500
	-1   0    0    1   
$EndComp
$Comp
L R R53
U 1 1 52F74297
P 6800 1700
F 0 "R53" H 6800 1750 40  0000 C CNN
F 1 "1k" H 6800 1650 40  0000 C CNN
F 2 "" H 6800 1700 60  0000 C CNN
F 3 "" H 6800 1700 60  0000 C CNN
	1    6800 1700
	0    1    1    0   
$EndComp
$Comp
L GND #PWR05
U 1 1 52F742BD
P 6600 1900
F 0 "#PWR05" H 6600 1900 30  0001 C CNN
F 1 "GND" H 6600 1830 30  0001 C CNN
F 2 "" H 6600 1900 60  0000 C CNN
F 3 "" H 6600 1900 60  0000 C CNN
	1    6600 1900
	1    0    0    -1  
$EndComp
$Comp
L LM386 U6
U 1 1 52F748ED
P 5850 2750
F 0 "U6" H 6000 2950 60  0000 C CNN
F 1 "LM386" H 6000 3050 60  0000 C CNN
F 2 "" H 5850 2750 60  0000 C CNN
F 3 "" H 5850 2750 60  0000 C CNN
	1    5850 2750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 52F748F3
P 5350 3350
F 0 "#PWR06" H 5350 3350 30  0001 C CNN
F 1 "GND" H 5350 3280 30  0001 C CNN
F 2 "" H 5350 3350 60  0000 C CNN
F 3 "" H 5350 3350 60  0000 C CNN
	1    5350 3350
	1    0    0    -1  
$EndComp
$Comp
L C C21
U 1 1 52F74902
P 5900 3300
F 0 "C21" H 5900 3350 40  0000 C CNN
F 1 "1uF" H 5900 3250 40  0000 C CNN
F 2 "" H 5900 3300 60  0000 C CNN
F 3 "" H 5900 3300 60  0000 C CNN
	1    5900 3300
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR07
U 1 1 52F7490B
P 5750 2300
F 0 "#PWR07" H 5750 2390 20  0001 C CNN
F 1 "+5V" H 5750 2390 30  0000 C CNN
F 2 "" H 5750 2300 60  0000 C CNN
F 3 "" H 5750 2300 60  0000 C CNN
	1    5750 2300
	1    0    0    -1  
$EndComp
$Comp
L C C23
U 1 1 52F74912
P 6500 2750
F 0 "C23" H 6500 2800 40  0000 C CNN
F 1 "10uF" H 6500 2700 40  0000 C CNN
F 2 "" H 6500 2750 60  0000 C CNN
F 3 "" H 6500 2750 60  0000 C CNN
	1    6500 2750
	1    0    0    -1  
$EndComp
$Comp
L R R49
U 1 1 52F74919
P 6600 2950
F 0 "R49" H 6600 3000 40  0000 C CNN
F 1 "100" H 6600 2900 40  0000 C CNN
F 2 "" H 6600 2950 60  0000 C CNN
F 3 "" H 6600 2950 60  0000 C CNN
	1    6600 2950
	0    -1   -1   0   
$EndComp
$Comp
L R R52
U 1 1 52F7491F
P 6700 2750
F 0 "R52" H 6700 2800 40  0000 C CNN
F 1 "2k2" H 6700 2700 40  0000 C CNN
F 2 "" H 6700 2750 60  0000 C CNN
F 3 "" H 6700 2750 60  0000 C CNN
	1    6700 2750
	-1   0    0    1   
$EndComp
$Comp
L R R54
U 1 1 52F74925
P 6800 2950
F 0 "R54" H 6800 3000 40  0000 C CNN
F 1 "1k" H 6800 2900 40  0000 C CNN
F 2 "" H 6800 2950 60  0000 C CNN
F 3 "" H 6800 2950 60  0000 C CNN
	1    6800 2950
	0    1    1    0   
$EndComp
$Comp
L GND #PWR08
U 1 1 52F74931
P 6600 3150
F 0 "#PWR08" H 6600 3150 30  0001 C CNN
F 1 "GND" H 6600 3080 30  0001 C CNN
F 2 "" H 6600 3150 60  0000 C CNN
F 3 "" H 6600 3150 60  0000 C CNN
	1    6600 3150
	1    0    0    -1  
$EndComp
$Comp
L CONN_5 P5
U 1 1 52F74BDD
P 2950 3200
F 0 "P5" V 2900 3200 50  0000 C CNN
F 1 "MIDI-IN" V 3000 3200 50  0000 C CNN
F 2 "" H 2950 3200 60  0000 C CNN
F 3 "" H 2950 3200 60  0000 C CNN
	1    2950 3200
	-1   0    0    1   
$EndComp
$Comp
L R R15
U 1 1 52F74BF4
P 3500 3100
F 0 "R15" H 3500 3150 40  0000 C CNN
F 1 "220" H 3500 3050 40  0000 C CNN
F 2 "" H 3500 3100 60  0000 C CNN
F 3 "" H 3500 3100 60  0000 C CNN
	1    3500 3100
	-1   0    0    1   
$EndComp
Wire Wire Line
	2600 2100 2600 2250
Wire Wire Line
	2750 2000 2750 2250
Wire Wire Line
	2600 2350 2600 2550
Wire Wire Line
	2600 2450 3200 2450
Wire Wire Line
	2750 2450 2750 2350
Connection ~ 2600 2450
Connection ~ 2750 2000
Connection ~ 2600 2100
Wire Wire Line
	2950 2100 4100 2100
Wire Wire Line
	3050 2100 3050 2250
Wire Wire Line
	3050 2450 3050 2350
Connection ~ 2750 2450
Wire Wire Line
	3200 2450 3200 2350
Connection ~ 3050 2450
Wire Wire Line
	3200 2250 3200 2000
Wire Wire Line
	3100 2000 4100 2000
Connection ~ 3200 2000
Connection ~ 3050 2100
Wire Wire Line
	4100 1900 4200 1900
Wire Wire Line
	4100 2150 4200 2150
Wire Wire Line
	4450 2350 4450 2500
Wire Wire Line
	4600 2350 4600 2400
Wire Wire Line
	4600 2400 4450 2400
Connection ~ 4450 2400
Wire Wire Line
	4300 2150 5050 2150
Wire Wire Line
	4450 2150 4450 2250
Wire Wire Line
	4600 1400 4600 2250
Wire Wire Line
	4600 1900 4300 1900
Wire Wire Line
	5350 1600 5350 2100
Wire Wire Line
	5350 1400 4600 1400
Connection ~ 4600 1900
Wire Wire Line
	6350 1500 6450 1500
Wire Wire Line
	6400 1500 6400 2050
Wire Wire Line
	6400 1950 5950 1950
Wire Wire Line
	5950 1950 5950 1900
Wire Wire Line
	5750 1900 5750 1950
Wire Wire Line
	5750 1950 5350 1950
Connection ~ 5350 1950
Wire Wire Line
	6400 2050 5950 2050
Connection ~ 6400 1950
Wire Wire Line
	5850 1900 5850 2050
Wire Wire Line
	5750 1050 5750 1100
Connection ~ 6400 1500
Wire Wire Line
	6550 1500 6650 1500
Wire Wire Line
	6750 1500 7000 1500
Wire Wire Line
	6800 1650 6800 1500
Connection ~ 6800 1500
Wire Wire Line
	6600 1650 6600 1500
Connection ~ 6600 1500
Wire Wire Line
	6800 1750 6800 1800
Wire Wire Line
	6800 1800 6600 1800
Wire Wire Line
	6600 1750 6600 1900
Connection ~ 6600 1800
Wire Wire Line
	5350 2850 5350 3350
Wire Wire Line
	6350 2750 6450 2750
Wire Wire Line
	6400 2750 6400 3300
Wire Wire Line
	6400 3200 5950 3200
Wire Wire Line
	5950 3200 5950 3150
Wire Wire Line
	5750 3150 5750 3200
Wire Wire Line
	5750 3200 5350 3200
Connection ~ 5350 3200
Wire Wire Line
	6400 3300 5950 3300
Connection ~ 6400 3200
Wire Wire Line
	5850 3150 5850 3300
Wire Wire Line
	5750 2300 5750 2350
Connection ~ 6400 2750
Wire Wire Line
	6550 2750 6650 2750
Wire Wire Line
	6750 2750 6900 2750
Wire Wire Line
	6800 2900 6800 2750
Connection ~ 6800 2750
Wire Wire Line
	6600 2900 6600 2750
Connection ~ 6600 2750
Wire Wire Line
	6800 3000 6800 3050
Wire Wire Line
	6800 3050 6600 3050
Wire Wire Line
	6600 3000 6600 3150
Connection ~ 6600 3050
Wire Wire Line
	5350 2650 5050 2650
Wire Wire Line
	5050 2650 5050 2150
Connection ~ 4450 2150
Wire Wire Line
	3550 3100 3650 3100
Wire Wire Line
	3350 3100 3450 3100
Wire Wire Line
	3400 3300 3700 3300
Wire Wire Line
	3400 3300 3400 3000
Wire Wire Line
	3400 3000 3350 3000
Text Label 4450 3150 0    60   ~ 0
MIDI_IN
$Comp
L D D12
U 1 1 52F758D1
P 3600 3200
F 0 "D12" V 3550 3300 60  0000 C CNN
F 1 "D" H 3605 3135 60  0001 C CNN
F 2 "" H 3600 3200 60  0000 C CNN
F 3 "" H 3600 3200 60  0000 C CNN
	1    3600 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3600 3100 3600 3150
Connection ~ 3600 3100
Wire Wire Line
	3600 3250 3600 3300
Connection ~ 3600 3300
$Comp
L CONN_3 K2
U 1 1 52F7A1A7
P 7450 2200
F 0 "K2" V 7400 2200 50  0000 C CNN
F 1 "CONN_3" V 7500 2200 40  0000 C CNN
F 2 "" H 7450 2200 60  0000 C CNN
F 3 "" H 7450 2200 60  0000 C CNN
	1    7450 2200
	1    0    0    1   
$EndComp
Wire Wire Line
	7100 2100 7000 2100
Wire Wire Line
	7000 2100 7000 1500
Wire Wire Line
	6900 2750 6900 2200
Wire Wire Line
	6900 2200 7100 2200
$Comp
L GND #PWR09
U 1 1 52F7A44E
P 7000 2400
F 0 "#PWR09" H 7000 2400 30  0001 C CNN
F 1 "GND" H 7000 2330 30  0001 C CNN
F 2 "" H 7000 2400 60  0000 C CNN
F 3 "" H 7000 2400 60  0000 C CNN
	1    7000 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 2300 7000 2300
Wire Wire Line
	7000 2300 7000 2400
Text Label 4850 1400 0    60   ~ 0
ROI2
Text Label 4800 2150 0    60   ~ 0
MOI2
Text Label 6100 1950 0    60   ~ 0
ROA
Text Label 6100 3200 0    60   ~ 0
MOA
Text Label 6600 1500 1    60   ~ 0
ROA2
Text Label 6600 2750 1    60   ~ 0
MOA2
Text Label 6900 2550 1    60   ~ 0
MOA3
Text Label 7000 1850 1    60   ~ 0
ROA3
$Comp
L ACPL-M21L U7
U 1 1 52F8C674
P 4100 3200
F 0 "U7" H 3900 3600 60  0000 C CNN
F 1 "ACPL-M21L" H 3750 2950 60  0001 C CNN
F 2 "" H 4100 3200 60  0000 C CNN
F 3 "" H 4100 3200 60  0000 C CNN
	1    4100 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3000 3650 3000
Wire Wire Line
	3650 3000 3650 3100
$Comp
L GND #PWR010
U 1 1 52F8C9F7
P 4100 3550
F 0 "#PWR010" H 4100 3550 30  0001 C CNN
F 1 "GND" H 4100 3480 30  0001 C CNN
F 2 "" H 4100 3550 60  0000 C CNN
F 3 "" H 4100 3550 60  0000 C CNN
	1    4100 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 3500 4100 3550
$Comp
L +3.3V #PWR011
U 1 1 52F8CBAF
P 4100 2750
F 0 "#PWR011" H 4100 2710 30  0001 C CNN
F 1 "+3.3V" H 4100 2860 30  0000 C CNN
F 2 "" H 4100 2750 60  0000 C CNN
F 3 "" H 4100 2750 60  0000 C CNN
	1    4100 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2750 4100 2800
Wire Wire Line
	4500 3150 4250 3150
NoConn ~ 3350 3200
NoConn ~ 3350 3300
NoConn ~ 3350 3400
Wire Wire Line
	4100 2000 4100 1900
Wire Wire Line
	4100 2100 4100 2150
$Comp
L CONN_6 P1
U 1 1 53C466CB
P 1550 1850
F 0 "P1" V 1500 1850 60  0000 C CNN
F 1 "CONN_6" V 1600 1850 60  0000 C CNN
F 2 "" H 1550 1850 60  0000 C CNN
F 3 "" H 1550 1850 60  0000 C CNN
	1    1550 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2150 1900 1900 1900
Text Label 2000 1900 0    60   ~ 0
MIDI_IN
$Comp
L GND #PWR012
U 1 1 53C469DB
P 2150 1600
F 0 "#PWR012" H 2150 1600 30  0001 C CNN
F 1 "GND" H 2150 1530 30  0001 C CNN
F 2 "" H 2150 1600 60  0000 C CNN
F 3 "" H 2150 1600 60  0000 C CNN
	1    2150 1600
	0    -1   -1   0   
$EndComp
$Comp
L +3.3V #PWR013
U 1 1 53C46A02
P 2250 1700
F 0 "#PWR013" H 2250 1660 30  0001 C CNN
F 1 "+3.3V" H 2250 1810 30  0000 C CNN
F 2 "" H 2250 1700 60  0000 C CNN
F 3 "" H 2250 1700 60  0000 C CNN
	1    2250 1700
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR014
U 1 1 53C46A29
P 2450 1800
F 0 "#PWR014" H 2450 1890 20  0001 C CNN
F 1 "+5V" H 2450 1890 30  0000 C CNN
F 2 "" H 2450 1800 60  0000 C CNN
F 3 "" H 2450 1800 60  0000 C CNN
	1    2450 1800
	0    1    1    0   
$EndComp
Wire Wire Line
	2450 1800 1900 1800
Wire Wire Line
	2250 1700 1900 1700
Wire Wire Line
	2150 1600 1900 1600
Wire Wire Line
	3000 2000 2300 2000
Wire Wire Line
	2300 2000 2300 2100
Wire Wire Line
	2300 2100 1900 2100
Wire Wire Line
	1900 2000 2150 2000
Wire Wire Line
	2150 2000 2150 2200
Wire Wire Line
	2150 2200 2400 2200
Wire Wire Line
	2400 2200 2400 2100
Wire Wire Line
	2400 2100 2850 2100
$EndSCHEMATC
