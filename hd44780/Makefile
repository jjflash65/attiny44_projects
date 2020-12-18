############################################################
#
#                         Makefile
#
############################################################

PROJECT    = txtlcd_demo


INC_DIR    = -I./ -I../include

# hier alle zusaetzlichen Softwaremodule angegeben

SRCS       = ../src/hd44780.o
SRCS      += ../src/my_printf.o
SRCS      += ../src/adc_single.o

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

