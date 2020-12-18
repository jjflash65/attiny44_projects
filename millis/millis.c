/* ----------------------------------------------------------
     millis.c

     Timerinterrupt zaehlt die long int Variable millis hoch
     (und stellt somit ein Arduino Pendant her)

     MCU     : attiny44
     F_CPU   : 8 MHz intern

     14.2.2018

   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"


#define led_init()      PA1_output_init()
#define led_set()       PA1_set()
#define led_clr()       PA1_clr()

volatile uint32_t millis = 0;

ISR (TIM1_COMPA_vect)
{
  millis++;
}

void timer1_init(void)
{
  TCCR1B = 1<<WGM12 | 1<<CS10;
  OCR1A = F_CPU / 1000;                 // 8000 = Reloadwert fuer 8 MHz
  TCNT1 = 0;

  TIMSK1 = 1<<OCIE1A;
  sei();
}

void tim1_delay(uint32_t dtime)
{
  volatile uint32_t now;

  now= millis;

  while(now + dtime > millis);
}

/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(void)
{
  // Mainprogram starts here
  timer1_init();
  led_init();

  while(1)
  {
    tim1_delay(500);
    led_set();
    tim1_delay(500);
    led_clr();
  }
}
