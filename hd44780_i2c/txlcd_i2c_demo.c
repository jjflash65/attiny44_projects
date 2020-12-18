/* ------------------------------------------------------------------
                             txlcd_i2c_demo.c

     Anbindung eines HS44780 kompatiblen Textdisplays ueber einen
     PCF8574 I2C I/O Expander

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


     23.10.2018 R. Seelig
   ------------------------------------------------------------------ */


#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avr_gpio.h"
#include "hd44780_i2c.h"
#include "my_printf.h"


#define printf        my_printf
#define delay         _delay_ms


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


/* ---------------------------------------------------------------------------------
                                      M-A-I-N
   ---------------------------------------------------------------------------------*/
int main(void)
{
  uint8_t i;

  txlcd_init();

  txlcd_setuserchar(0,&charbmp0[0]);
  txlcd_setuserchar(1,&charbmp1[0]);
  txlcd_setuserchar(2,&ohmbmp[0]);

  gotoxy(1,1);
  printf("ATtiny44-PCF8574");
  while(1)
  {
    gotoxy(1,2);
    printf("   Hallo Welt   ");
    delay(1500);
    gotoxy(1,2);
    printf("Userchar: %c %c %c",0,1,2);
    delay(1500);
  }
}
