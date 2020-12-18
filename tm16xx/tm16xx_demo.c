/* ----------------------------------------------------------
                      tm1651_demo.c

   Grundsaetzliche Funktionen des TM16xx mit 4 stelliger
   7-Segmentanzeige


     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung

     ATtiny44         TM16xx
     ---------------------------
        PB1            CLK
        PB0            DIO

     PB0 und PB1 sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen

     09.01.2019 R. Seelig
   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>

#include "tm16xx.h"
#include "avr_gpio.h"

#define delay     _delay_ms




/* ----------------------------------------------------------
                            MAIN
   ---------------------------------------------------------- */
void main(void)
{
  int16_t i, i2;
  uint8_t b;

  hellig= 11;
  tm16_init();

  while(1)
  {
    for (i= 10; i> -1; i--)               // Countdown-Zaehler
    {
      tm16_setdez(i,3);                   // Dezimalpunkt an
      _delay_ms(500);
      tm16_setdez(i,0);                   // Dezimalpunkt aus
      _delay_ms(500);
    }

    for (i= 0; i< 3; i++)                 // 0000 blinken lassen
    {
      tm16_clear();
      _delay_ms(500);
      tm16_setdez(0,0);
      _delay_ms(500);
    }

    for (i= 3; i> -1; i--)                // Ziffer wandern lassen
    {
      tm16_clear();
      tm16_setzif(i,0);
      _delay_ms(300);
    }

    tm16_sethex(0x1b3a);                  // Hex-Wert anzeigen
    _delay_ms(2000);

    tm16_clear();
    for (i2= 0; i2< 5; i2++)             // Segmentlauflicht, 20 Durchlaeufe
    {
      for (i= 0; i< 6; i++)
      {
        tm16_setseg(0,i);
        tm16_setseg(3,i);
        _delay_ms(100);
      }
    }

    // Tasten als Code anzeigen, Taste 3 wiederholt Demo
    do
    {
      b= tm16_readkey();
      tm16_setdez(b,0);

      _delay_ms(50);
    } while (b != 3);
    _delay_ms(50);
    while( tm16_readkey() == 3);
  }
}
