/* -------------------------------------------------------
                         tft_demo.c

     Demoprogramm fuer GLCD mit Aufloesung 128 x 128 Pixel

     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz

     27.08.2017  R. Seelig
   ------------------------------------------------------ */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "tftdisplay.h"
#include "my_printf.h"

#define printf            my_printf
#define delay             _delay_ms

#define settextattr(nr)   { textcolor= rgbfromega(nr & 0x0f);            \
                            bkcolor = rgbfromega((nr >> 4) & 0x0f); }

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
#define  u_in                 52500
#define  bin2spg(value)       ( value * ( u_in / 1023 ) / 100 )

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  lcd_putchar(ch);
}

/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main(void)
{
  #define xofs    6
  #define yofs    5

  uint16_t adc_value;

  lcd_init();
  clrscr();
  adc_init(2, 3);                          // Analogeingang auf PA3

  printfkomma= 2;
  outmode= 3;

  textcolor= rgbfromega(yellow);
  gotoxy(1,1);
  printf("ATtiny44 - ADC");
  textcolor= rgbfromega(lightblue);
  gotoxy(1, yofs+1);
  printf("PA3:         V");
  settextattr(0x71);
  gotoxy(xofs, yofs); printf("      ");
  gotoxy(xofs, yofs+2); printf("      ");
  while(1)
  {
    adc_init(2, 3);                          // Analogeingang auf PA3
    adc_value= bin2spg(adc_getvalue() );
    gotoxy(xofs, yofs+1);
    if (adc_value > 0)
      printf(" %k ", adc_value );
    else
      printf(" 0.00 ");
    delay(500);
  }
}
