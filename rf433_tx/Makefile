############################################################
#
#                         Makefile
#
############################################################


# Projektnummer: 0: am433_rxdemo
#                1: am433_txdemo

PROJECT_NR = 1

ifeq ($(PROJECT_NR), 0)
	PROJECT    = am433_rxdemo
endif

ifeq ($(PROJECT_NR), 1)
	PROJECT    = am433_txdemo
endif


INC_DIR    = -I./ -I../include

# hier alle zusaetzlichen Softwaremodule angegeben

SRCS       =  ../src/am433_transceiv.o

PRINT_FL   = 0
SCAN_FL    = 0
MATH       = 0


ifeq ($(PROJECT_NR), 0)
# -------------------------------
#            Receiver
# -------------------------------


# 	fuer Compiler / Linker
	FREQ       = 8000000ul
	MCU        = attiny44

# 	fuer AVRDUDE
	PROGRAMMER = stk500v2
	PROGPORT   = /dev/ttyUSB0
	BRATE      = 115200
	DUDEOPTS   = -B1

endif

ifeq ($(PROJECT_NR), 1)
# -------------------------------
#            Transmitter
# -------------------------------


        # 	fuer Compiler / Linker
	FREQ       = 8000000ul
	MCU        = attiny44

# 	fuer AVRDUDE
	PROGRAMMER = usbasp
	PROGPORT   =
	BRATE      =
	DUDEOPTS   = -B1
endif


# bei manchen Mainboards muss ein CH340G (USB zu seriell Chip) evtl. geresetet werden!
CH340RESET = 0

include ../makefile.mk

