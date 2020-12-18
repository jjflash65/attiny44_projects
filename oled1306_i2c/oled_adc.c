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
#include "adc_single.h"

#define  delay                _delay_ms
#define  printf               my_printf

/* -------------------------------------------------------
                      u_in und Makro bin2spg

    u_in:
    Spannung in 0,1 mV, die vor einem (eventuellen)
    Spannungsteiler am ADC den 10 Bit Maximalwert (1023)
    erzeugt

    bin2spg:
    rechnet den Wert von u_in bei einer Aufloesung von
    10 Bit in einen Festkommazahlen - Spannungswert um.
    Wert von bspw. 382 entspricht hier dann 3.82 Volt

    Hinweis: Nach dieser Methode koennen bei Messbereich
    5V max. 2% Fehler durch Rundung entstehen
   ------------------------------------------------------- */
#define  u_in                 52000
#define  bin2spg(value)       ( value * ( u_in / 1023 ) / 100 )

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

  adc_init(2, 3);                          // Analogeingang auf PA3

  gotoxy(2,0);
  printf("Tiny44 - ADC");
  doublechar= 1;
  gotoxy(11,3); oled_putchar('V');
  while(1)
  {
    gotoxy(2,3);
    printf("%k", bin2spg(adc_getvalue()) );
    delay(400);
  }
}
