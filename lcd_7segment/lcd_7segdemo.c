/* -------------------------------------------------------
                        lcd_7segdemo.c

     Anbindung eines zweistelligen 7-Segment LCD's OHNE
     eigenen Controller ("pures Display") ueber
     2 kaskadierteSchieberegister SN74HC595

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegung siehe lcd_7segdemo.h

     05.11.2018  R. Seelig
   ------------------------------------------------------ */


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "lcd_7seg.h"

#define  delay     _delay_ms


/* ---------------------------------------------------------------------------------
                                      M-A-I-N
   ---------------------------------------------------------------------------------*/
int main(void)
{
  uint8_t cnt = 0;

  lcd7s_init();

  while(1)
  {
    for (cnt= 10; cnt != 0; cnt--)
    {
      lcd7s_dezout(cnt);
      delay(1000);
    }
    for (cnt= 0; cnt < 10; cnt++)
    {
      lcd7s_dezout(cnt);
      delay(1000);
    }
  }
}
