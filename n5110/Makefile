###############################################################################
#
#                                 Makefile
#
###############################################################################


PROJECT   = n5110_demo

SRCS      = ../src/n5110.o
SRCS     += ../src/font5x7.o
SRCS     += ../src/my_printf.o

PRINTF_FL = 0
SCANF_FL  = 0
MATH      = 0

# fuer Compiler / Linker
FREQ      = 8000000ul
MCU       = attiny44

# fuer AVRDUDE
PROGRAMMER = stk500v2
SERPORT    = /dev/ttyUSB0
BRATE      = 115200


include ../makefile.mk

