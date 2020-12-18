###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = oled_adc

SRCS      = ../src/my_printf.o
SRCS     += ../src/oled1306_spi.o
SRCS     += ../src/font8x8h.o
SRCS     += ../src/adc_single.o

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

