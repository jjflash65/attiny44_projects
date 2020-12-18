/* -------------------------------------------------------
                           hx1838.h

     Header fuer Softwaremodul zum HX1838 IR-Receiver

     Belegt zur Auswertung des Receivers Timer1, IR-
     Receiver loest am angeschlossenen Pin einen
     Pinchangen Interrupt aus

     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz
     Fuses :  fuer 8 MHz intern
              lo 0xe2
              hi 0xdf

     24.09.2018  R. Seelig
   ------------------------------------------------------ */

#ifndef in_hx1838_d
  #define in_hx1838_d

  #include <avr/io.h>
  #include <avr/interrupt.h>

  #include "avr_gpio.h"

/* -------------------------------------------------------
            Define zum Pinchange Interrupt

    fuer PCINT0..7  (PA0..PA7)
      PCMSK0, PCIE0

    fuer PCINT8..11 (PB0..PB4)
      PCMSK1, PCIE1

    Hinweis:
      wird im Main-Programm die serielle Schnitt-
      stelle benoetigt, so ist grundsatzlich PCINT8..11
      zu waehlen, da der PCINT0_vect bereits von
      usiuart belegt ist
  ------------------------------------------------------ */

  // PB0 als IR-Data-Input (PCINT8)
  #define ir_input_init()    PB2_input_init();
  #define is_irin()          is_PB2()

  #define IR_ISR_vect        PCINT1_vect
  #define IR_PCINT           PCINT10
  #define IR_PCMSK           PCMSK1
  #define IR_PCIE            PCIE1

  #define tim1_getvalue()    ( (uint16_t) ( (TCNT1H << 8) | TCNT1L ))
  #define tim1_clr()         { TCNT1H= 0;  TCNT1L= 0; }

  extern volatile uint16_t  ir_code;                                          // Code des letzten eingegangenen 16-Bit Wertes
  extern volatile uint8_t   ir_newflag;                                       // zeigt an, ob ein neuer Wert eingegangen ist

  void hx1838_init(void);

#endif
