/* -----------------------------------------------------------
                         test_max7219.c

   Demoprogramm zur 8x8 Leuchtdiodenmatrix mit MAX7219
   (Chinamodul)


   Benoetigte Hardware;

            - 8x8 LED Matrix mit MAX7219 IC

   MCU   :  ATtiny44
   Takt  :  intern oder extern


   21.09.18 by R. Seelig

   Pins und Anschlussbelegung siehe max7219.h
   ----------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avr_gpio.h"
#include "max7219_dot8x8.h"

#define delay    _delay_ms


static const uint8_t smiley[8] PROGMEM =  {
  0x3c, 0x42, 0x95, 0xa1, 0xa1, 0x95, 0x42, 0x3c };

uint16_t wuerfel[6] =
  { 0x0400, 0x8020, 0x8420, 0xa0a0, 0xa4a0, 0xaaa0 };


/* -----------------------------------------------------------
                               i2fb
                        Integer to framebuffer

     einen 16-Bit Integerwert als 4x4 Pixel grosses Bitmap
     in einen Framebuffer einsetzen.

     Uebergabe:

        *dest  : Adresse des Framebuffers
        bmp4x4 : 4x4 grosses Bitmap
        xpos   : linke obere Ecke der Ausgabeposition
        ypos   : untere rechte Ecke der Ausgabeposition

   ----------------------------------------------------------- */
void i2fb(uint8_t *dest, uint16_t bmp4x4, uint8_t xpos, uint8_t ypos)
{
  uint8_t a;
  uint8_t x, y;

  x= 0; y= 0;
  for (a= 15; a!= 0; a--)
  {
    if (bmp4x4 & (1 << a)) fbuf_putpixel(dest, x + xpos, y + ypos, 1);
                      else fbuf_putpixel(dest, x + xpos, y + ypos, 0);
    x++;
    x= x % 4;
    if (!(x)) y++;
  }
  m7219_setbmp(dest);
}

/* -----------------------------------------------------------
                             M-A-I-N
   ----------------------------------------------------------- */
int main(void)
{
  uint8_t cx;
  uint8_t x, y, z;


  m7219_init();                         // Anschluesse des STM8 die die Matrix steuern
                                        // als Ausgang setzen und die Register des
                                        // MAX7219 konfigurieren

  while(1)                              // endlose Demowiederholung
  {

    // Wuerfel anzeigen
    fbuf_clr(&fbuf[0]);
    for (z= 1; z< 7; z++)
    {
      i2fb(&fbuf[0], wuerfel[z-1], 1, 0);
      delay(350);
    }
    for (z= 1; z< 7; z++)
    {
      i2fb(&fbuf[0], wuerfel[(z+2) % 6], 3, 4);
      delay(350);
    }

    delay(2000);

    fbuf_clr(&fbuf[0]);                 // Anzeigebuffer loeschen
    i2fb(&fbuf[0], wuerfel[(5-1) % 6], 3, 2);

    // "rotierendes" Lauflicht
    for (cx= 0; cx < 2; cx++)
    {
      // obere und untere Reihe gegenlaeufiges  Lauflicht
      for (x= 0; x< 8; x++)
      {
        // Pixel im Framebuffer setzen
        fbuf_putpixel(&fbuf[0], x, 0, 1);
        fbuf_putpixel(&fbuf[0], 7-x, 7, 1);
        // und diesen anzeigen
        m7219_setbmp(&fbuf[0]);

        delay(70);

        // Pixel im Framebuffer setzen
        fbuf_putpixel(&fbuf[0], x, 0, 0);
        fbuf_putpixel(&fbuf[0], 7-x, 7, 0);
        // und diesen anzeigen
        m7219_setbmp(&fbuf[0]);
      }

      // vordere und hintere Spalte gegenlaeufiges Lauflicht
      for (y= 1; y< 7; y++)
      {
        // Pixel im Framebuffer setzen
        fbuf_putpixel(&fbuf[0], 7, y, 1);
        fbuf_putpixel(&fbuf[0], 0, 7-y, 1);
        // und diesen anzeigen
        m7219_setbmp(&fbuf[0]);

        delay(70);

        // Pixel im Framebuffer setzen
        fbuf_putpixel(&fbuf[0], 7, y, 0);
        fbuf_putpixel(&fbuf[0], 0, 7-y, 0);
        // und diesen anzeigen
        m7219_setbmp(&fbuf[0]);
      }
    }
    // Ausgangsposition des Lauflichts anzeigen (als letzten Schritt des Lauflichts)
    fbuf_clr(&fbuf[0]);
    fbuf_putpixel(&fbuf[0], 0, 0, 1);
    fbuf_putpixel(&fbuf[0], 7, 7, 1);
    m7219_setbmp(&fbuf[0]);
    delay(1000);

    fbuf_clr(&fbuf[0]);
    m7219_clrscr();

    // einen Text auf der Matrix scrollen
    fbuf_scroll_text_in(&fbuf[0],"wuerfeln", 100, 400);
    delay(1000);

    // "Smiley" blinken lassen
    for (cx= 0; cx< 6; cx++)
    {
      m7219_clrscr();
      delay(200);
      m7219_setpgmbmp(&smiley[0]);
      delay(200);
    }
    delay(1000);

  }
}
