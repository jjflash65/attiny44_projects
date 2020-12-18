/* ----------------------------------------------------------
                          ws2812.c

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegung siehe ws2812.h

     08.11.2018  R. Seelig
   ---------------------------------------------------------- */

#include "ws2812.h"

#define delay     _delay_ms

const uint8_t PROGMEM egapalette[] =
{
/* -------------------------------------------------------------
      Farbpalette fuer dunklere EGA Farben (siehe EGA-
      Farbzuweisungen
   ------------------------------------------------------------- */
  0x00,0x00,0x00, 0x00,0x00,0x03, 0x00,0x02,0x00, 0x00,0x03,0x03,       // 0..3
  0x03,0x00,0x00, 0x03,0x00,0x03, 0x03,0x01,0x00, 0x07,0x07,0x07,       // 4..7
  0x01,0x01,0x01, 0x01,0x01,0x06, 0x00,0x09,0x00, 0x01,0x06,0x06,       // 8..11
  0x08,0x00,0x00, 0x06,0x01,0x06, 0x07,0x02,0x00, 0x08,0x08,0x08,       // 11..15
/* -------------------------------------------------------------
      Farbpalette fuer helle EGA Farben
   ------------------------------------------------------------- */
  0x00,0x00,0x00, 0x00,0x00,0xaa, 0x00,0xaa,0x00, 0x00,0xaa,0xaa,       // 16..19
  0xaa,0x00,0x00, 0xaa,0x00,0xaa, 0xaa,0x55,0x00, 0xaa,0xaa,0xaa,       // 20..23
  0x55,0x55,0x55, 0x55,0x55,0xff, 0x05,0xff,0x05, 0x05,0xff,0xff,       // 24..27
  0xff,0x00,0x00, 0xbb,0x00,0xff, 0xff,0x60,0x00, 0xff,0xff,0xff        // 28..31
};


/* ----------------------------------------------------------
                            rgbfromega

     beschreibt eine struct Variable auf die *f zeigt mit
     einem Farbwert, der aus der EGA-Palette entnommen wird

     Uebergabe:
                     nr   : Farbeintrag der EGA-Palette
                     *f   : Zeiger auf die zu beschreibende
                            struct Variable
   ---------------------------------------------------------- */
void rgbfromega(uint8_t eganr, struct colvalue *f)
{
  (*f).g= pgm_read_byte(&egapalette[(eganr*3)+1]);     // gruen
  (*f).r= pgm_read_byte(&egapalette[(eganr*3)]);       // rot
  (*f).b= pgm_read_byte(&egapalette[(eganr*3)+2]);        // blau
}

/* ----------------------------------------------------------
                          rgbfromvalue

     beschreibt eine struct Variable auf die *f zeigt mit
     einem RGB Farbwert

     Uebergabe:
                  r,g,b   : RGB - Farbwert
                     *f   : Zeiger auf die zu beschreibende
                            struct Variable

     Usage:       //  Farbe Lila nach rgbcol
                  rgbfromvalue(128, 0, 128, &rgbcol);

   ---------------------------------------------------------- */
void rgbfromvalue(uint8_t r, uint8_t g, uint8_t b, struct colvalue *f)
{
  (*f).r= r;
  (*f).g= g;
  (*f).b= b;
}

/* ----------------------------------------------------------
                        ws_output

      externe Assemblerrotuine in ws_output.S

      gibt einen Pufferspeicher auf der WS2812 LED-Kette
      aus.

      Uebergabe:
                *ptr  : Zeiger auf ein Array, das angezeigt
                        werden soll
                anz   : Anzahl der anzuzeigenden Bytes (fuer
                        jede LED muessen 3 Bytes ausgegeben
                        werden, so muss bspw. fuer eine 6er
                        Kette hier der Wert 18 stehen)
   ---------------------------------------------------------- */
extern void ws_output(uint8_t *ptr, uint16_t count);

/* ----------------------------------------------------------
                        ws_showbuffer

      gibt ein Array das die Farbinformationen fuer die LEDs
      beinhaltet auf der WS2812 LED-Kette aus

      Uebergabe:
                *ptr  : Zeiger auf ein Array, das angezeigt
                        werden soll
                anz   : Anzahl der anzuzeigenden LED's
   ---------------------------------------------------------- */
void ws_showbuffer(uint8_t *ptr, uint16_t count)
{
  ws_output(ptr, count*3);
  _delay_us(250);
}

/* ----------------------------------------------------------
                        ws_setrgbcol

      setzt den Farbwert der in

                    struct colvalue f

      angegeben ist, in ein Array an Stelle nr ein. Dieses
      Array kann mittels ws_showbuffer auf einen LED-Strang
      ausgegeben werden kann.

      Parameter:
                    *ptr : Zeiger auf ein Array, das die
                           RGB Farbwerte aufnehmen soll
                      nr : LED-Nummer im Array, welche die
                           Position im Leuchtdiodenstrang
                           repraesentiert
      struct colvalue *f : Zeiger auf einen RGB-Farbwert

      Usage:
                      rgbfromvalue(50,0,50, &rgbcol);
                      ws_setrgbcol(&ledbuffer[0], 4, &rgbcol);

   ---------------------------------------------------------- */
void ws_setrgbcol(uint8_t *ptr, uint16_t nr, struct colvalue *f)
{
  ptr+= (nr*3);
  *ptr= (*f).g;
  ptr++;
  *ptr= (*f).r;
  ptr++;
  *ptr= (*f).b;
}

/* ----------------------------------------------------------
                          ws_reset

     setzt die Leuchtdiodenreihe (fuer einen folgenden
     Datentransfer zu den Leuchtdioden) zurueck
   ---------------------------------------------------------- */
void ws_reset(void)
{
  ws_port |= (1 << ws_portpin);         // Data-Leitung high
  ws_port &= ~(1 << ws_portpin);        // Data-Leitung low
  delay(2);
}

/* ----------------------------------------------------------
                          ws_init

     initialisiert den Pin, an den die WS2812 LED-Kette an-
     geschlossen ist als Ausgang und fuehrt einen Reset der
     Kette durch.
   ---------------------------------------------------------- */
void ws_init(void)
{
  ws_ddr |= (1 << ws_portpin);
  ws_reset();
}

/* ----------------------------------------------------------
                          ws_clrarray
     loescht ein LED Anzeigearray

     Parameter:
                   *ptr : Zeiger auf ein Array, das
                          geloescht werden soll
                    anz : Anzahl der LEDs, die angezeigt
                          werden sollen
   ---------------------------------------------------------- */
void ws_clrarray(uint8_t *ptr, int anz)
{
  memset(ptr, 0x00, anz*3);
}

/* ----------------------------------------------------------
                          ws_clrarray

     Verzoegerungsschleife fuer die Aufblendfunktionen
     ws_blendup_left / ws_blendup_right

     Parameter:
                   dtime : Verzoegerungswert (absolut unbe-
                           stimmt, ca. 1,6 mS per Einheit)
   ---------------------------------------------------------- */
void ws_delay(uint16_t dtime)
{
  volatile uint16_t dt;

  for (dt= 0; dt< dtime; dt++)
  {
    delay(1);
  }
}

/* ----------------------------------------------------------
                          ws_blendup_left

      blendet eine LED-Anzahl anz mit dem Farbwert, der in


                      struct colvalue f

      angegeben ist, links schiebend auf. Hierfuer wird das
      Array, auf das *ptr zeigt mit dem Farbwert aufgefuellt.

      Verzoegerungszeit dtime bestimmt die Dauer eines
      Einzelschrittes beim Aufbau in Millisekunden.

      Usage:
                      // blendet 7 lila LEDS auf
                      ledanz= 7;
                      rgbfromvalue(50,0,50, &rgbcol);
                      ws_blendup_left(&ledbuffer[0], ledanz, &rgbcol, 200);

   ---------------------------------------------------------- */
void ws_blendup_left(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime)
{
  int8_t            i;

  ws_reset();
  for (i= 1; i< anz+1; i++)
  {
    ws_setrgbcol(ptr, i-1, f);
    ws_showbuffer(ptr, anz);
    ws_delay(dtime);
  }
}

/* ----------------------------------------------------------
                          ws_blendup_right

      blendet eine LED-Anzahl anz mit dem Farbwert, der in


                       struct colvalue f

      angegeben ist, rechts schiebend auf. Hierfuer wird das
      Array, auf das *ptr zeigt mit dem Farbwert aufgefuellt.

      Verzoegerungszeit dtime bestimmt die Dauer eines
      Einzelschrittes beim Aufbau in Millisekunden.
   ---------------------------------------------------------- */
void ws_blendup_right(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime)
{
  int8_t i;

  ws_reset();
  for (i= anz; i> -1; i--)
  {
    ws_setrgbcol(ptr, i-1, f);
    ws_showbuffer(ptr, anz);
    ws_delay(dtime);
  }
}

/* ----------------------------------------------------------
                          ws_buffer_rl

      rotiert einen Pufferspeicher der die Leuchtdiodenmatrix
      enthaelt um eine LED-Position nach links (also 3 Bytes)
      und fuegt hierbei die am Ende anstehende LED am Anfang
      wieder ein.
   ---------------------------------------------------------- */
void ws_buffer_rl(uint8_t *ptr, uint8_t lanz)
{
  uint8_t b, b2, j;
  int     i;
  uint8_t *hptr1, *hptr2;

  for (j= 0; j< 3; j++)
  {
    hptr1 = ptr;
    hptr1 += (lanz*3)-1;
    b= *hptr1;
    for (i= (lanz*3)-1; i> 0; i--)
    {
      hptr2= ptr;
      hptr2 += (i-1);
      b2= *hptr2;
      hptr2++;
      *hptr2= b2;
    }
    *ptr= b;
  }
}

/* ----------------------------------------------------------
                          ws_buffer_rr

      rotiert einen Pufferspeicher der die Leuchtdiodenmatrix
      enthaelt um eine LED-Position nach rechts (also 3 Bytes)
      und fuegt hierbei die am Anfang anstehende LED am Ende
      wieder ein.
   ---------------------------------------------------------- */
void ws_buffer_rr(uint8_t *ptr, uint8_t lanz)
{
  uint8_t b, b2, j;
  int     i;
  uint8_t *hptr1, *hptr2;


  for (j= 0; j< 3; j++)
  {
    b= *ptr;
    for (i= 0; i < (lanz*3)-1;  i++)
    {
      hptr2= ptr;
      hptr2 += i+1;
      b2= *hptr2;
      hptr2--;
      *hptr2= b2;
    }
    hptr1 = ptr;
    hptr1 += (lanz*3)-1;
    *hptr1= b;
  }
}



