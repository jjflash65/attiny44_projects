###############################################################################
#
#                                 Makefile
#
#   einfaches Makefile zum "builden" von HEX-Dateien fuer Atmel (c) AVR-
#   Mikrocontroller.
#
#
#
#   Januar 2019,  R. Seelig
#
###############################################################################

# Projectnummer
#                 1 : Demo
#                 2 : Integer Calculator

PROJ_NR           = 2

ifeq ($(PROJ_NR),1)
	PROJECT           = tm1638_demo
endif

# Project 2 funktioniert nur mit Board 2. Umstellen der Boardversion in
# tm1638.h nicht vergessen !!!!!
ifeq ($(PROJ_NR),2)
	PROJECT           = tm1638_calc
endif

SRCS              = ../src/tm1638.o

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

