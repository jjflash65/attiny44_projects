/* -------------------------------------------------------
                      txtlcd_demo.c

     Testprogramm fuer ein Standardtextdisplay mit
     HD44780 kompatiblen LC-Controller, 4-Bit Daten-
     transfermodus

     Hardware : Text-LCD

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     17.09.2018  R. Seelig

   ------------------------------------------------------- */

/*
      Anschluss Pollin-Display C0802-04 an den Controller
      ---------------------------------------------------
         o +5V
         |                            Display                   ATtiny44
         _                        Funktion   PIN            PIN    Funktion
        | |
        |_|                          GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
        _|_                           RS      4 ------------  9      PB6
        \ /                          GND      5 ------------
        -'-                    (Takt) E       6 ------------  8      PB4
         |                           D4       7 ------------  2      PB3
        --- GND                      D5       8 ------------  3      PB2
                                     D6       9 ------------  6      PB1
                                     D7      10 ------------  7      PB0

*/
// ------------------------------------------------------------------------------------------------------------------


#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "hd44780.h"
#include "my_printf.h"

#define printf    my_printf
#define delay     _delay_ms

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



static const unsigned char charbmp0[8] PROGMEM =         // hochgestelltes "o" (fuer Gradangaben)
  { 0x0c, 0x12, 0x12, 0x0c, 0x00, 0x00, 0x00, 0x00 };

static const unsigned char charbmp1[8] PROGMEM =         // Pfeil nach links
  { 0x08, 0x0c, 0x0e, 0x0f, 0x0e, 0x0c, 0x08, 0x00 };

static const unsigned char ohmbmp[8] PROGMEM =           // Ohmzeichen
  { 0x0e, 0x11, 0x11, 0x11, 0x0a, 0x0a, 0x1b, 0x00};

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  txlcd_putchar(ch);
}


/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main()
{
  uint16_t adc_value;

  txlcd_init();

  txlcd_setuserchar(0,&charbmp0[0]);
  txlcd_setuserchar(1,&charbmp1[0]);
  txlcd_setuserchar(2,&ohmbmp[0]);

  gotoxy(1,1); printf("UserChar");
  gotoxy(1,2); printf("%c %c %c", 0,1,2);

  delay(2000);
  txlcd_init();
  gotoxy(2,1); printf("ADC");
  gotoxy(7,2); my_putchar('V');
  printfkomma= 2;
  while(1)
  {
    adc_init(2, 3);                          // Analogeingang auf PA3
    adc_value= bin2spg(adc_getvalue() );
    gotoxy(2,2);
    if (adc_value > 0)
      printf("%k ", adc_value );
    else
      printf(" 0.00 ");
    delay(500);
  }
}
