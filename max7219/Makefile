###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = dot8x8_demo

SRCS      = ../src/max7219_dot8x8.o
SRCS     += ../src/font8x8h.o

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

