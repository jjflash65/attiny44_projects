/* ----------------------------------------------------------

   Das "Hallo Welt" der Microcontroller: Blinkende LED

   Hardware :  - LED

   MCU      :  Attiny44
   Takt     :  8 MHz intern

   Fuses    :  Lo:0xE2    Hi:0xDF

   14.08.2018 R. Seelig
   ---------------------------------------------------------- */
/*

                                                 ATtiny44
                                             Anschlusspins IC

                                              +-----------+
                                         Vcc  | 1      14 |  GND
                   PCINT8 - XTAL1 - CLKI PB0  | 2   A  13 |  PA0 - ADC0 - AREF - PCINT0
                        PCINT9 - XTAL2 - PB1  | 3   T  12 |  PA1 - ADC1 - AIN0 - PCINT1
                 PCINT11 - /reset - dW - PB3  | 4   t  11 |  PA2 - ADC2 - AIN1 - PCINT2
         PCINT10 - INT0 - OC0A - CKOUT - PB2  | 5   i  10 |  PA3 - ADC3 - T0 - PCINT3
            PCINT7 - ICP - OC0B - ADC7 - PA7  | 6   n   9 |  PA4 - ADC4 - USCK - SCL - T1 - PCINT4
PCINT6 - OC1A - SDA - MOSI - DI - ADC6 - PA6  | 7   y   8 |  PA5 - ADC5 - DO - MISO - OC1B PCINT5
                                              +-----------+
*/


#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"


#define led_init()     PB0_output_init()
#define led_clr()      PB0_set()
#define led_set()      PB0_clr()


#define blinkspeed     500


int main(void)
{

  led_init();

  while(1)
  {
    led_set();
    _delay_ms(blinkspeed);
    led_clr();
    _delay_ms(blinkspeed);
  }
}
