/* -----------------------------------------------------------
                         max7219_dot8x8.c

   Softwaremodul fuer eine  Ansteuerung des Multpilex IC
   MAX7219 in Verbindung mit einer 8x8 LED Matrix
   (China-Modul)

   Benoetigte Hardware;

            - 8x8 LED Matrix mit MAX7219 IC

   MCU   :  ATtiny44
   Takt  :  extern oder extern


   24.09.2018 by R. Seelig

   Pins und Anschlussbelegung siehe max7219_dot8x8.h
   ----------------------------------------------------------- */

#include "max7219_dot8x8.h"

// "Framebuffer" der ein komplettes 8x8 Pixelbitmap aufnehmen kann
uint8_t fbuf[8] =
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* -----------------------------------------------------------
     Helpers
   ----------------------------------------------------------- */

/* -----------------------------------------------------------
     vdelay

     Warteschleife mit einem ungefaehren Wert value * 1,1mS
   ----------------------------------------------------------- */
void vdelay(uint16_t value)
{
  volatile uint16_t i;
  for (i= 0; i< value; i++) _delay_ms(1);
}

/* -----------------------------------------------------------
     reversbyte

     Tauscht Bitwerte von LSB und MSB:
       B7 xchange B0
       B6 xchange B1
       B5 xchange B2
       B4 xchange B5
   ----------------------------------------------------------- */
uint8_t reversebyte (uint8_t x)
{
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}


/* -----------------------------------------------------------
     Kommunikation
   ----------------------------------------------------------- */

/* -----------------------------------------------------------
     serout

     serielle Datenausgabe mittels Bitbanging, MSB zuerst
   ----------------------------------------------------------- */
void serout(uint8_t data)
{
  uint8_t i, val;

  for(i= 8; i> 0; i--)
  {
    val = (data & (1 << (i-1)) ) >> (i - 1);
    m7219_clkclr();
    if(val)
      m7219_dinset();
    else
      m7219_dinclr();
    m7219_clkset();
  }
}

/* -----------------------------------------------------------
     m7219_send

     Datenuebertragung zu einem MAX7219

        reg  : zu beschreibendes Register des MAX7219
        data : Wert der in das Register des MAX7219
               geschrieben wird.
   ----------------------------------------------------------- */
void m7219_send(uint8_t reg, uint8_t data)
{
  m7219_loadclr();
  serout(reg);
  serout(data);
  m7219_loadset();
}

/* -----------------------------------------------------------
     m7219_init

     Setzt die Anschluesse an die der MAX7219 angeschlossen ist
     als Ausgaenge und intitalisiert die Register des ICs
   ----------------------------------------------------------- */
void m7219_init()
{
  m7219_init_con();

  m7219_dinclr();
  m7219_clkclr();
  m7219_loadclr();

  m7219_send(0x0b, 0x07);   // Scanlimit
  m7219_send(0x09, 0x00);   // LED-Matrix, keine 7 Segment
  m7219_send(0x0c, 0x01);   // Power an
  m7219_send(0x0f, 0x00);   // kein Displaytest
  m7219_send(0x0a, 0x02);   // Intensitaet

  m7219_clrscr();
}

/* -----------------------------------------------------------
     m7219_col

     Setzt einen 8 Bit-Wert ( = 8 "Pixel") in eine Spalte
     der LED-Matrix.

        digit : Spalte, die beschrieben werden soll
        data  : Wert, mit der die Spalte beschrieben wird
                (8 Pixel)
   ----------------------------------------------------------- */
void m7219_col(uint8_t digit, uint8_t data)
{
  m7219_send(digit + 1, data);
}

/* -----------------------------------------------------------
     m7219_clrscr

     loescht die gesamte Matrixanzeige
   ----------------------------------------------------------- */
void m7219_clrscr()
{
  uint8_t i;

  for(i= 0; i< 8; i++)
  {
    m7219_col(i, 0);
  }
}

/* -----------------------------------------------------------
     m7219_setbmp

     Zeichnet ein gesamtes 8x8 Pixel grosses "Bitmap* auf die
     LED-Matrix.

        *bmp  : Zeiger auf 8 Bytes ( = 64 Bits) grossen
                Buffer. Dieses Bitmap wird angezeigt.
   ----------------------------------------------------------- */
void m7219_setbmp(uint8_t *bmp)
{
  uint8_t i;

  for (i= 0; i< 8; i++)
  {
    m7219_col(i, bmp[i]);
  }
}

/* -----------------------------------------------------------
     m7219_setpgmbmp

     Zeichnet ein gesamtes 8x8 Pixel grosses "Bitmap", das im
     Programmspeicher liegt, auf die LED-Matrix.

        *bmp  : Zeiger auf 8 Bytes ( = 64 Bits) grossen
                Buffer. Dieses Bitmap wird angezeigt.
   ----------------------------------------------------------- */
void m7219_setpgmbmp(const uint8_t *bmp)
{
  uint8_t i;

  for (i= 0; i< 8; i++)
  {
    m7219_col(i, pgm_read_byte(&bmp[i]));
  }
}

/* -----------------------------------------------------------
     Framebuffer
   ----------------------------------------------------------- */

/* -----------------------------------------------------------
     fbuf_clr

     loescht einen 8 Byte grossen Pufferspeicher in dem
     alle 64 Bits geloescht werden
   ----------------------------------------------------------- */
void fbuf_clr(uint8_t *dest)
{
  uint8_t i;

  for (i= 0; i< 8; i++) dest[i]= 0;
}


/* -----------------------------------------------------------
     fbuf_putpixel

     setzt in einem 8 Byte grossen Pufferspeicher ein
     einzelnes Bit (Pixel)

     Parameter

        *bmp  : Zeiger auf einen 8 Byte grossen Pufferspeicher
        x     : X-Koordinate des zu setzenden Pixels
        y     : Y-Koordinate des zu setzenden Pixels
        c     : "Farbe" = 1 : LED leuchtet
                        = 0 : LED aus
   -----------------------------------------------------------*/
void fbuf_putpixel(uint8_t *dest, uint8_t x, uint8_t y, uint8_t c)
{
  if (c)
    dest[x] |= (1 << y);
  else
    dest[x] &= ~(1 << y);
}

/* -----------------------------------------------------------
     fbuf_shl_ins

     verschiebt alle Spalten des Zielspeichers *dest um eine
     Stelle nach links und fuegt an der rechten Stelle  eine
     Spalte aus src ein
   ----------------------------------------------------------- */
void fbuf_shl_ins(uint8_t *dest, uint8_t src)
{
  uint8_t i;

  for (i= 0; i< 7; i++) dest[i]= dest[i+1];
  dest[7]= src;
}

/* -----------------------------------------------------------
     fbuf_scroll_in

     "Scrollt" ein 8x8 grosses Image von links in die LED-
     matrix, die in *dest enthalten ist, ein.

       *dest : Zielimage (das letztendlich angezeigt wird)
       *src  : einzuscrollendes Image
       dtime : Scrollgeschwindigkeit in mS
   ----------------------------------------------------------- */

void fbuf_scroll_in(uint8_t *dest, uint8_t *src, int dtime)
{
  uint8_t cx;

  for (cx= 0; cx< 8; cx++)
  {
    fbuf_shl_ins(dest, src[cx]);
    m7219_setbmp(dest);
    vdelay(dtime);
  }
}

/* -----------------------------------------------------------
     fbuf_scroll_text_in

     Scrollt einen Text in die Matrix ein und wartet nach
     jedem eingescrollten Buchstaben eine Zeitdauer, bevor
     der naechste Buchstabe eingeschoben wird

       *txt   : Zeiger auf den Text, der auf der LED-Matrix
                angezeigt werden soll
       dtime1 : Zeidauer in mS zwischen den Scrollschritten
       dtime2 : Zeitdauer in mS die ein Buchstabe auf der
                LED-Matrix angezeigt wird, bevor der naechste
                Buchstabe eingescrollt wird.
   ----------------------------------------------------------- */
void fbuf_scroll_text_in(uint8_t *dest, uint8_t *txt, int dtime1, int dtime2)
{
  uint8_t c, b;
  uint8_t tmp_buf[8];

  do
  {
    c = *txt;
    if (c)
    {
      for (b= 0; b< 8; b++)
      {
        tmp_buf[b]= reversebyte(pgm_read_byte(&font8x8h[c-32][b]));
      }
      fbuf_scroll_in(dest, &tmp_buf[0], dtime1);
      vdelay(dtime2);
    }
  }while ( *txt++ );

}

