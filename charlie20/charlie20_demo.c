/* -------------------------------------------------------
                        charlie20_demo.c

     Demoprogramm fuer Ansteuerung von 20 LED's mittels
     Charlieplexing

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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "avr_gpio.h"
#include "charlie20.h"

#define speed    30
#define speed2   70
#define delay    _delay_ms

/* ------------------------------------------------------
                           M-A-I-N
   ------------------------------------------------------ */
int main(void)
{
  uint32_t  counter;
  uint32_t  cx;

  charlie20_init();
  charlie20_buf= 0x90aa3;

  delay(2000);

  counter= 1;
  while(1)
  {
    do
    {
      charlie20_buf= counter;
      counter= counter << 1ul;
      delay(speed);
    } while (counter< 0xfffff);

    counter= counter >> 2;

    do
    {
      charlie20_buf= counter;
      counter= counter >> 1ul;
      delay(speed);
    } while (counter> 0);


    counter= 1;
    cx= 1;

    do
    {
      cx= cx << 1ul;
      charlie20_buf += cx;
      counter++;
      delay(speed2);
    } while(counter < 20);

    counter= 1;
    cx= 1;

    do
    {
      charlie20_buf -= cx;
      cx= cx << 1ul;
      counter++;
      delay(speed2);
    } while(counter < 20);

    counter= 2;

  }
}
