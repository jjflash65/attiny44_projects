###############################################################################
#
#                                 Makefile
#
#   einfaches Makefile zum "builden" von HEX-Dateien fuer Atmel (c) AVR-
#   Mikrocontroller.
#
#
#
#   Mai 2017,  R. Seelig
#
###############################################################################

PROJECT           = swserial_demo

SRCS              = ./softuart.o
SRCS             += ../src/my_printf.o

INCLUDE_PATHS     = -I. -I../include


PRINTF_FL         = 0
SCANF_FL          = 0
MATH              = 0

# fuer Compiler / Linker
FREQ              = 8000000ul
MCU               = attiny44

# fuer AVRDUDE
PROGRAMMER        = usbasp
SERPORT           = /dev/ttyUSB0
BRATE             = 115200
DUDEOPTS          = B1


include ../makefile.mk

