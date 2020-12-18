###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT   = oled_rotate

SRCS      = ../src/i2c_sw.o
SRCS     += ../src/my_printf.o
SRCS     += ../src/oled1306rot_i2c.o
SRCS     += ../src/font8x8h.o

PRINTF_FL = 0
SCANF_FL  = 0
MATH      = 0

# fuer Compiler / Linker
FREQ      = 8000000ul
MCU       = attiny44

# fuer AVRDUDE
PROGRAMMER = usbasp
PROGPORT   = /dev/ttyUSB0
BRATE      = 115200
DUDEOPTS   = -B1


include ../makefile.mk

