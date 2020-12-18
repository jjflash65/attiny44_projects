/* ----------------------------------------------------------
     vfd_samsung.c

     Demoprogramm fuer Display VFD-20T201DA2 von Samsung.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     04.01.2019  R. Seelig
   ---------------------------------------------------------- */

  /* ----------------------------------------------------------
        Anschluesse
          Display       AVR-Controller

        1 --- Vcc
        2 --- clk  --------- PB0
        3 --- Gnd
        4 --- data --------- PB1
        5 --- rst  --------- PA1

        (definiert in vfd_20t201.h)
     ---------------------------------------------------------- */


#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avr_gpio.h"
#include "vfd_20t201.h"
#include "my_printf.h"


#define delay                 _delay_ms
#define  printf               tiny_printf


static const unsigned char eurobmp[5] PROGMEM =
  { 0x28, 0x7c, 0xaa, 0x82, 0x40 };

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  vfd_putchar(ch);
}

/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(void)
{
  uint8_t i;

  vfd_init();
  vfd_brightness(255);
  clrscr();
  vfd_setuserchar(0x18,&eurobmp[0]);

  while(1)
  {
    gotoxy(3,1);
    printf("Eurozeichen: %c",0x18);
    gotoxy(5,0);
    printf("Hallo Welt");
    delay(500);
    gotoxy(5,0);
    printf("          ");
    delay(500);
  }
}
