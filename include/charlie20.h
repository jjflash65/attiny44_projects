/* -------------------------------------------------------
                         charlie20.h

     Header fuer Softwaremodul zur Ansteuerung von
     20 LED's mittels Charlieplexing

     Hardware : 20 LED
     MCU      : ATtiny44
     F_CPU    : 8 MHz intern

     Fuses    : fuer 8 MHz intern
                  lo 0xe2
                  hi 0xdf

     Fuses    : fuer ext. Quarz >= 8 MHz
                  lo 0xde
                  hi 0xdf

     18.09.2018    R. Seelig
   ------------------------------------------------------ */


/* -------------------------------------------------------------------------------
                        Charlieplexing 20-LED Modul

     Beim Charlieplexing sind jeweils 2 antiparallel geschaltete Leuchtdioden
     an 2 GPIO-Leitungen angeschlossen. Hieraus ergeben sich 10 Paare zu jeweils
     2 LEDs (bei 5 verwendeten GPIO-Anschluessen).

     Bsp.:

      A  o------+-----,         C o------+-----,
                |     |                  |     |
               ---   ---                ---   ---
             A \ /   / \ B            C \ /   / \ D
             B ---   --- A            D ---   --- C
                |     |                  |     |
      B  o------+-----'         D o------+-----'


      A  B  B  C  C  D  D  E  A  C  C  E  D  B  A  D  A  E  E  B   Linenkombination
      B  A  C  B  D  C  E  D  C  A  E  C  B  D  D  A  E  A  B  E
      ----------------------------------------------------------
      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19  LED-Nummern
   ------------------------------------------------------------------------------- */

#ifndef in_charlie20
  #define in_charlie20

  #include <avr/io.h>
  #include <avr/interrupt.h>

  #include "avr_gpio.h"

  // Zuordnung LEDs zu GPIO-Anschluessen

  #define charlieA_output()  PA0_output_init()
  #define charlieA_input()   { DDRA &= ~MASK0; PORTA &= ~(MASK0); }
  #define charlieA_set()     PA0_set()
  #define charlieA_clr()     PA0_clr()

  #define charlieB_output()  PA1_output_init()
  #define charlieB_input()   { DDRA &= ~MASK1; PORTA &= ~(MASK1); }
  #define charlieB_set()     PA1_set()
  #define charlieB_clr()     PA1_clr()

  #define charlieC_output()  PA2_output_init()
  #define charlieC_input()   { DDRA &= ~MASK2; PORTA &= ~(MASK2); }
  #define charlieC_set()     PA2_set()
  #define charlieC_clr()     PA2_clr()

  #define charlieD_output()  PA4_output_init()
  #define charlieD_input()   { DDRA &= ~MASK4; PORTA &= ~(MASK4); }
  #define charlieD_set()     PA4_set()
  #define charlieD_clr()     PA4_clr()

  #define charlieE_output()  PA5_output_init()
  #define charlieE_input()   { DDRA &= ~MASK5; PORTA &= ~(MASK5); }
  #define charlieE_set()     PA5_set()
  #define charlieE_clr()     PA5_clr()

  /* -------------------------------------------------------
                          Prototypen
     ------------------------------------------------------- */
  void charlie20_init(void);

  /* -------------------------------------------------------
       wichtigte globale Variable:
       Ein Beschreiben dieser Variablen fuehrt bei durch-
       gefuehrte Initialisierung zum Anzeigen des Bitmusters
       des Inhaltes von charlie20_buf
     ------------------------------------------------------- */
  extern uint32_t      charlie20_buf;             // Buffer in dem das Bitmuster aufgenommen wird,
                                                  // welches ueber den Timerinterrupt auf den LEDs
                                                  // ausgegeben wird

  /* -------------------------------------------------------
                        Benutzermakros
        einzelnes Bit im 32-Bit Integer setzen/loeschen.
        Bedeutet gleichzeitig das setzen einzelner LEDs im
        Charlieplexing
     ------------------------------------------------------- */
  #define charlie20_bufset(nr)   (charlie20_buf |=  ( (uint32_t)1 << (uint32_t)nr ))
  #define charlie20_bufclr(nr)   (charlie20_buf &= ~( (uint32_t)1 << (uint32_t)nr ))

#endif
