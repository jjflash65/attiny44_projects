# ======================================================================
# Makefile for USB SPI interface
#
# Copyright 2006-2008 Dick Streefland
#
# This is free software, licensed under the terms of the GNU General
# Public License as published by the Free Software Foundation.
# ======================================================================

MCUTYPE         = attiny44

USBTINY		= ../usbtiny
TARGET_ARCH	= -mmcu=$(MCUTYPE)
OBJECTS		= main.o
FLASH_CMD	= avrdude -c stk500v2 -P /dev/ttyUSB0 -b 115200 -B5 -p$(MCUTYPE) -U flash:w:main.hex
FUSES_CMD	= avrdude -c stk500v2 -P /dev/ttyUSB0 -b 115200 -B5 -p$(MCUTYPE) -U hfuse:w:0xdf:m -U lfuse:w:0xef:m
STACK		= 44
FLASH		= 4096
SRAM		= 128
SCHEM		= spi.ps

include $(USBTINY)/common.mk
