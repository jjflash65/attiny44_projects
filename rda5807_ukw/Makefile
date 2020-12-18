###############################################################################
#
#                                 Makefile
#
###############################################################################

PROJECT_NR   = 2

ifeq ($(PROJECT_NR),1)

PROJECT   = rda5807_uart

SRCS      = ../src/my_printf.o
SRCS     += ../src/usiuart.o
SRCS     += ../src/i2c_sw.o
SRCS     += ../src/rda5807.o

endif

ifeq ($(PROJECT_NR),2)

PROJECT   = rda5807_display

SRCS     += ../src/i2c_sw.o
SRCS     += ../src/rda5807.o
SRCS     += ../src/oled1306rot_i2c.o
#SRCS     += ../src/oled1306_i2c.o
SRCS     += ./font_reduced.o

endif

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

