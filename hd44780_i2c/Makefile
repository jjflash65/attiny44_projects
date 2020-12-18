###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = txlcd_i2c_demo

SRCS      = ../src/i2c_sw.o
SRCS     += ../src/hd44780_i2c.o
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

