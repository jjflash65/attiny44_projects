/* ----------------------------------------------------------
                      digit4_demo.c

   Grundsaetzliches zur 4 stelligen 7-Segmentanzeige mit
   TM1637 Treiber - IC

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung

     ATtiny44     7-Segmentmodul
     ---------------------------
        PA0            CLK
        PA1            DIO

     PA0 und PA1 sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen


   14.09.2018 R. Seelig
   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "seg7_tm1637.h"


/*
    4-stelliges 7-Segmentanzeigemodul mit TM1637

    Anzeigenposition 0 ist das linke Segment des Moduls

         +---------------------------+
         |  POS0  POS1   POS2  POS3  |
         |  +--+  +--+   +--+  +--+  | --o  GND
         |  |  |  |  | o |  |  |  |  | --o  +5V
         |  |  |  |  | o |  |  |  |  | --o  DIO
         |  +--+  +--+   +--+  +--+  | --o  CLK
         |      4-Digit Display      |
         +---------------------------+
*/


/* ----------------------------------------------------------
                            MAIN
   ---------------------------------------------------------- */
void main(void)
{
  int16_t i, i2;

  hellig= 11;
  tm1637_init();

  while(1)
  {
    for (i= 10; i> -1; i--)               // Countdown-Zaehler
    {
      tm1637_dp= 1;                       // Doppelpunkt an
      tm1637_setdez(i);
      _delay_ms(500);
      tm1637_dp= 0;                       // Doppelpunkt aus
      tm1637_setdez(i);
      _delay_ms(500);
    }

    for (i= 0; i< 3; i++)                 // 0000 blinken lassen
    {
      tm1637_clear();
      _delay_ms(500);
      tm1637_setdez(0);
      _delay_ms(500);
    }

    for (i= 3; i> -1; i--)                // Ziffer wandern lassen
    {
      tm1637_clear();
      tm1637_setzif(i,0);
      _delay_ms(300);
    }

    tm1637_sethex(0x1b3a);                // Hex-Wert anzeigen
    _delay_ms(2000);

    tm1637_clear();
    for (i2= 0; i2< 20; i2++)             // Segmentlauflicht, 20 Durchlaeufe
    {
      for (i= 0; i< 6; i++)
      {
        tm1637_setseg(0,i);
        tm1637_setseg(3,i);
        _delay_ms(100);
      }
    }
  }
}
