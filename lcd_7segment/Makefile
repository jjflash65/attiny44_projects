############################################################
#
#                         Makefile
#
############################################################

# Project 0:    lcd_7segdemo
#         1:    lcd_7seg_therm

PROJECT_NR = 1

ifeq ($(PROJECT_NR), 0)
	PROJECT    = lcd_7segdemo
	# hier alle zusaetzlichen Softwaremodule angegeben

	SRCS       = ../src/lcd_7seg.o
endif

ifeq ($(PROJECT_NR), 1)
	PROJECT    = lcd_7seg_therm
	# hier alle zusaetzlichen Softwaremodule angegeben

	SRCS       = ../src/lcd_7seg.o
	SRCS      += ../src/adc_single.o
endif

INC_DIR    = -I./ -I../include


PRINT_FL   = 0
SCAN_FL    = 0
MATH       = 0

# fuer Compiler / Linker
FREQ       = 8000000ul
MCU        = attiny44

# fuer AVRDUDE
PROGRAMMER = usbasp
PROGPORT   = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B1

# bei manchen Mainboards muss ein CH340G (USB zu seriell Chip) evtl. geresetet werden!
CH340RESET = 0

include ../makefile.mk

