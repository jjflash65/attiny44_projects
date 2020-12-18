/* ------------------------------------------------------------------
                              oled_test.c

     Funktionstest eines OLED Displays 128x64 Pixel, I2C-Interface
     und SSD1306 Displaycontroller.

     Zeigt die Spannung, die nach einem Spannungsteiler
     1M zu 22k an PA3 anliegt, an.


     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL

     Pinbelegung ADC
     ---------------

     PA3 = analoger Eingang


     12.09.2018 R. Seelig
   ------------------------------------------------------------------ */

/*
                                                 ATtiny44
                                        4 kByte Flash / 256 Byte SRAM

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
#include "i2c_sw.h"
#include "my_printf.h"
#include "oled1306_i2c.h"

#define  delay                _delay_ms
#define  printf               my_printf


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  oled_putchar(ch);
}

/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main(void)
{
  uint16_t adc_value;

  printfkomma= 2;

  ssd1306_init();
  clrscr();

  gotoxy(0,0);
  printf("Tiny44 - ADC");
  gotoxy(0,2);
  printf("Grosser Text");
  doublechar= 1;
  gotoxy(1,3);
  printf("Text");
  while(1)
  {
  }
}
