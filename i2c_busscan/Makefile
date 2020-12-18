###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = i2c_scan

SRCS      = ../src/i2c_sw.o
SRCS     += ../src/usiuart.o
SRCS     += ../src/my_printf.o

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

