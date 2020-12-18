###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = sr04_timer

SRCS      = ../src/i2c_sw.o
SRCS     += ../src/oled1306rot_i2c.o
SRCS     += ../src/font8x8h.o
SRCS     += ../src/toene.o

PRINTF_FL = 0
SCANF_FL  = 0
MATH      = 0

# fuer Compiler / Linker
FREQ      = 8000000ul
MCU       = attiny44

# fuer AVRDUDE
PROGRAMMER = usbasp
SERPORT    = /dev/ttyUSB0
BRATE      = 115200


include ../makefile.mk

