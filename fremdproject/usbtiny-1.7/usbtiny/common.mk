# ======================================================================
# Common Makefile for USBtiny applications
#
# Macros to be defined before including this file:
#
# USBTINY	- the location of this directory
# TARGET_ARCH	- gcc -mmcu= option with AVR device type
# OBJECTS	- the objects in addition to the USBtiny objects
# FLASH_CMD	- command to upload main.hex to flash
# FUSES_CMD	- command to program the fuse bytes
# STACK		- maximum stack size (optional)
# FLASH		- flash size (optional)
# SRAM		- SRAM size (optional)
# SCHEM		- Postscript version of the schematic to be generated
#
# Copyright 2006-2010 Dick Streefland
#
# This is free software, licensed under the terms of the GNU General
# Public License as published by the Free Software Foundation.
# ======================================================================

CC	= avr-gcc
OPTIM	= -Os -ffunction-sections $(-fno-split-wide-types)
CFLAGS	= -g -Wall -I. -I$(USBTINY) $(OPTIM)
LDFLAGS	= -g -Wl,--relax,--gc-sections
MODULES = crc.o int.o usb.o $(OBJECTS)


all:	clean main.hex

clean:
	rm -f main.elf *.o *.hex

main.elf:	$(MODULES)
	$(LINK.o) -o $@ $(MODULES)

main.hex:	main.elf size
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

size:
	avr-size -C main.elf --mcu=$(MCUTYPE)

flash:
	$(FLASH_CMD)

fuses:
	$(FUSES_CMD)

crc.o:		$(USBTINY)/crc.S $(USBTINY)/def.h usbtiny.h
	$(COMPILE.c) $(USBTINY)/crc.S
int.o:		$(USBTINY)/int.S $(USBTINY)/def.h usbtiny.h
	$(COMPILE.c) $(USBTINY)/int.S
usb.o:		$(USBTINY)/usb.c $(USBTINY)/def.h $(USBTINY)/usb.h usbtiny.h
	$(COMPILE.c) $(USBTINY)/usb.c

main.o:		$(USBTINY)/usb.h
