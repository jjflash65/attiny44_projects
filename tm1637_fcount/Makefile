############################################################
#
#                         Makefile
#
############################################################

PROJECT    = tm16_fcount_2


INC_DIR    = -I./ -I../include

# hier alle zusaetzlichen Softwaremodule angegeben

SRCS       = ../src/tm16xx.o
SRCS      += ../src/adc_single.o

PRINT_FL   = 0
SCAN_FL    = 0
MATH       = 0

# fuer Compiler / Linker
FREQ       = 16000000ul
MCU        = attiny44

# fuer AVRDUDE
PROGRAMMER = stk500v2
PROGPORT   = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B1

# bei manchen Mainboards muss ein CH340G (USB zu seriell Chip) evtl. geresetet werden!
CH340RESET = 0

include ../makefile.mk

