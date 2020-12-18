############################################################
#
#                         Makefile
#
############################################################

PROJECT    = seg7_hc595_demo


INC_DIR    = -I./ -I../include

# hier alle zusaetzlichen Softwaremodule angegeben

SRCS       = ../src/seg7_hc595.o

PRINT_FL   = 0
SCAN_FL    = 0
MATH       = 0

# fuer Compiler / Linker
FREQ       = 8000000ul
MCU        = attiny44

# fuer AVRDUDE
PROGRAMMER = stk500v2
PROGPORT   = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B1

# bei manchen Mainboards muss ein CH340G (USB zu seriell Chip) evtl. geresetet werden!
CH340RESET = 0

include ../makefile.mk

