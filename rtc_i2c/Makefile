############################################################
#
#                         Makefile
#
############################################################

PROJECT    = realtime_demo


INC_DIR    = -I./ -I../include

# hier alle zusaetzlichen Softwaremodule angegeben

SRCS       = ../src/i2c_sw.o
SRCS      += ../src/oled1306rot_i2c.o
SRCS      += ../src/font8x8h.o
SRCS      += ../src/rtc_i2c.o

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
CH340RESET = 1

include ../makefile.mk

