/* ----------------------- n5110_demo.c ---------------------

     Testprogramm fuer ein Display eines Nokia Displays an
     einem ATtiny44 MCU. Aufgrund sehr begrenzter Speicher-
     kapazitaet des Tiny's ist keine Grafik moeglich !!!


     MCU   : ATtiny 44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     13.09.2018 R. Seelig
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
Hinweis:

    bei Verwendung von USI als Hardware SPI ist der sendende Ausgang
    D0. Das heisst: PA5 entspricht hier dann MOSI (im Gegensatz zur
    Verwendung von ISP bei der der Anschluss PA5 MISO ist) !!!!


    Wiring:

       ATTiny44             Display
    ---------------------------------

       PA4  (9)   ---       CLK (5)
       PA5  (8)   ---       DIN (4)
       PA3 (13)   ---       DC  (3)
       PA2 (11)   ---       CE  (2)
       PA1 (12)   ---       RST (1)
                            VCC (6)
                            LED (7)
                            GND (8)

*/

#include <util/delay.h>
#include <avr/io.h>

#include "n5110.h"
#include "my_printf.h"

#define  printf      tiny_printf
#define  delay       _delay_ms

#define speed     200


void my_putchar(char c)
{
  lcd_putchar_d(c);
}


int main(void)
{
  uint8_t i;

  delay(100);
  // lcd_init(contrast, bias, temperatur)
  lcd_init(39, 6, 2);

  gotoxy(0,1);
  printf("printf-Ausgabe");
  printfkomma= 1;

  for (i= 100; i> 0; i--)
  {
    gotoxy(3,3);
    printf("i= %d  ", i);
    delay(100);
  }
  clrscr();


  gotoxy(0,1);
  prints(" LCD-5110\n\r ------------");
  prints("\n\r     MCU:\n\r   ATtiny44");
  while(1)
  {
    for (i= 0; i<14; i++)
    {
      gotoxy(i,0); lcd_putchar_d('o');
      gotoxy(13-i,5); lcd_putchar_d('o');
      delay(speed);
      gotoxy(i,0); lcd_putchar_d(' ');
      gotoxy(13-i,5); lcd_putchar_d(' ');
    }
    for (i= 1; i< 5; i++)
    {
      gotoxy(13,i); lcd_putchar_d('o');
      gotoxy(0,5-i); lcd_putchar_d('o');
      delay(speed);
      gotoxy(13,i); lcd_putchar_d(' ');
      gotoxy(0,5-i); lcd_putchar_d(' ');

    }
  }

}
