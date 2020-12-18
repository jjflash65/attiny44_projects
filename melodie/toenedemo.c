/*  --------------------------------------------------------
                        toenedemo.c

     Spielt den Schneewalzer auf dem Portpin PA1

     Versuch    : diverses

     Compiler: AVR-GCC 4.7.2

     17.06.2014        R. Seelig

  -------------------------------------------------------- */

#include <util/delay.h>                                    // beinhaltet _delay_ms(char) und _delay_us(char)
#include <avr/io.h>                                        // fuer Behandlung der GPIO Pins... GPIO = general purpose input output
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "toene.h"

#define  delay    _delay_ms

static const unsigned char jingle01[] PROGMEM = { "g8g8g8h8o+e4d3" };

static const unsigned char schneew [] PROGMEM = { "c4d4e2g4e2g4e1d4e4f2g4f2g4f1g4a4h2+f4-h2+f4-h1a4h4+c2e4c2-a4g1e4g4+c1-h1+d1c1-a2+c4-a2+c4-a1" };


int main()
{
  int i;
  PA1_output_init();

  toene_init();
  playstring(&schneew[0]);
  _delay_ms(1000);
  playstring(&jingle01[0]);

  while(1);
}
