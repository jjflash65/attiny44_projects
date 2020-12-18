/* ----------------------------------------------------------
                         ws2812_demo2.c

     Demoprogramm fuer WS2812 Leuchtdiodenkette.

     Hardware : 12 Stck. WS2812 LEDS im Ring angeordnet

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     09.11.2018  R. Seelig


   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ws2812.h"

#define delay    _delay_ms


// Anzahl der LEDs im Leuchtdiodenstrang
#define ledanz       12

// der Pufferspeicher, der die Farbwerte rot, gruen, blau fuer die
// einzelnen LED's aufnimmt
uint8_t ledbuffer[ledanz * 3];

// rgbcol nimmt Farbwerte fuer rot, gruen und blau auf
struct colvalue rgbcol;


// Geschwindigkeit fuer Aufblenden und Rotieren
#define blendspeed    80
#define rolspeed      45
#define rainspeed     150

// Makros um Schreibarbeit zu sparen, die Funktionen beziehen sich hier fest
// auf <ledbuffer[0]> fuer den Pufferspeicher und auf <rgbcol> fuer die
// Farbvariable

#define setrgbcol(pos)    ( ws_setrgbcol(&ledbuffer[0], pos, &rgbcol) )
#define blendup_left()    ( ws_blendup_left(&ledbuffer[0], ledanz, &rgbcol, blendspeed))
#define blendup_right()   ( ws_blendup_right(&ledbuffer[0], ledanz, &rgbcol, blendspeed))
#define buffer_rl()       ( ws_buffer_rl(&ledbuffer[0], ledanz) )
#define buffer_rr()       ( ws_buffer_rr(&ledbuffer[0], ledanz) )
#define showbuffer()      ( ws_showbuffer(&ledbuffer[0], ledanz) )
#define clrarray()        ( ws_clrarray(&ledbuffer[0], ledanz) )


// RGB-Farbwerte fuer Regenbogenfarben
const uint8_t rainbowcolor[] =
{
    255,3,3,  80,10,10, 153,102,117,  72,0,224,
  1,159,232, 0,255,000,   255,255,0, 255,219,0,
  255,146,0, 255,073,0,   255,0,255, 160,0,160
};

/* ----------------------------------------------------------
                             M-A-I-N
   ---------------------------------------------------------- */
int main(void)
{
  int8_t  cnt,i;
  uint16_t ci1, ci2;

  ws_init();

  clrarray();
  showbuffer();

  while(1)
  {
    for (i= 0; i< 12; i++)
    {
      rgbfromega(ws_blue, &rgbcol);
      setrgbcol(i);
      showbuffer();
      delay(80);
    }
    for (i= 0; i< 12; i++)
    {
      rgbfromega(ws_black, &rgbcol);
      setrgbcol(i);
      showbuffer();
      delay(80);
    }
  }

  while(1);
  {
    // EGA Farben nacheinander links und rechts auf und abblenden

    for (i= 9; i< 16; i++)
    {
      rgbfromega(i+16, &rgbcol);                                       // Farbwert aus EGA-Palette holen
      if (i &0x01)
        blendup_left();                                                // und links aufblenden
      else
        blendup_right();                                               // rechts aufblenden

      rgbfromega(0, &rgbcol);                                          // Farbe "aus"
      if (i &0x01)
        blendup_left();                                                // und links ausblenden
      else
        blendup_right();                                               // rechts ausblenden
    }

    // Farben setzen fuer Farbrotation
    rgbfromvalue(0x01, 0x00, 0x00, &rgbcol);
    blendup_left();                                                    // "Hintergrundfarbe" aufblenden

    rgbfromvalue(0x00, 0x00, 0x20, &rgbcol);
    setrgbcol(0);                                                      // Farbe Position 0 setzen
    showbuffer();
    delay(500);

    rgbfromvalue(0x00, 0x00, 0xff, &rgbcol);
    setrgbcol(1);                                                      // dto. Position 1
    showbuffer();
    delay(500);

    rgbfromvalue(0xff, 0xff, 0xff, &rgbcol);
    setrgbcol(2);                                                      // dto. Position 2
    showbuffer();
    delay(500);

    // und rotieren lassen
    for (i= 0; i< 36; i++)
    {
      delay(rolspeed);
      buffer_rl();
      showbuffer();
    }

    // Regenbogenfarben setzen (und beim Setzen anzeigen)
    clrarray();
    for (i= 0; i< ledanz*3; i+= 3)
    {
      rgbcol.r= pgm_read_byte(&rainbowcolor[i]);
      rgbcol.g= pgm_read_byte(&rainbowcolor[i+1]);
      rgbcol.b= pgm_read_byte(&rainbowcolor[i+2]);
      setrgbcol(i/3);
      showbuffer();
      delay(100);
    }

    // Farben rotieren lassen
    for (i= 0; i< 36; i++)
    {
      buffer_rl();
      showbuffer();
      delay(rainspeed);
    }
    // Regenbogenfarben ausblenden
    rgbfromvalue(0x00, 0x00, 0x00, &rgbcol);
    blendup_left();
    clrarray();
    showbuffer();

  }

  while(1);
}
