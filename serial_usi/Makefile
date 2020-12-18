###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = usi_serial_demo

SRCS      = ../src/my_printf.o
SRCS     += ../src/usiuart.o
SRCS     += ../src/adc_single.o

PRINTF_FL = 0
SCANF_FL  = 0
MATH      = 0

# fuer Compiler / Linker
FREQ      = 8000000ul
MCU       = attiny84

# fuer AVRDUDE
PROGRAMMER = usbasp
SERPORT    = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B1


include ../makefile.mk

