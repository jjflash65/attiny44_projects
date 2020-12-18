###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = ir_receiver_demo

SRCS      = ../src/my_printf.o
SRCS     += ../src/usiuart.o
SRCS     += ../src/hx1838.o

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
DUDEOPTS   = -B1


include ../makefile.mk

