/* ----------------------------------------------------------
     vfd_20t201.h

     Softwaremodul fuer Vakuumfluoreszenz Display
     VFD-20T201DA2 von Samsung.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     04.01.2019  R. Seelig

   ---------------------------------------------------------- */

#include "vfd_20t201.h"

/* ----------------------------------------------------------
     vfd_init

     Initialisiert die fuer den Datentransfer benutzten Pins
     als Ausgaenge, setzt clk und data auf high und fuert
     einen Reset des Displays durch
   ---------------------------------------------------------- */
void vfd_init(void)
{
  vfdpin_init();
  vfdclk_set();
  vfddat_set();
  vfdrst_set();
  _delay_ms(1);
  vfdrst_clr();
  _delay_ms(10);
}

/* ----------------------------------------------------------
     vfd_send

     sendet einen 8-Bit Wert an das VFD Display
   ---------------------------------------------------------- */
void vfd_send(uint8_t value)
{
  int8_t i;

  for (i= 7; i>= 0; i--)
  {
    if ((value >> i) & 1 )  vfddat_set(); else vfddat_clr();

    _delay_us(20);
    vfdclk_clr();
    _delay_us(20);
    vfdclk_set();
    _delay_us(20);
  }

  _delay_ms(1);
}

/* ----------------------------------------------------------
     vfd_brightness

     stellt die Helligkeit der Textanzeige ein.

     Uebergabe:
          value  ---- 0x01 = 10%
                      0xff = 100%
   ---------------------------------------------------------- */
void vfd_brightness(uint8_t value)
{
  vfd_send(0x04);
  vfd_send(value);
}

/* -------------------------------------------------------
     vfd_setuserchar

     kopiert die Bitmap eines benutzerdefiniertes Zeichen
     in den Charactergenerator des Displays

               nr : Position im Ram des Displays, an
                    der die Bitmap hinterlegt werden
                    soll.
                    Zulaessige Werte fuer nr sind
                    0x18..0x1F
        *userchar : Zeiger auf die Bitmap des Zeichens

   Organisation der Characterbytes:

           Byte1 : A7  A6  A5  A4  A3  A2  A1  x
           Byte2 : A14 A13 A12 A11 A10 A9  A8  x
           Byte3 : A21 A20 A19 A18 A17 A16 A15 x
           Byte4 : A28 A27 A26 A25 A24 A23 A22 x
           Byte5 : A35 A34 A33 A32 A31 A30 A29 x

           Beispiel Eurozeichen

            A1  A8  A15 A22 A29      .  .  x  x  .
            A2  A9  A16 A23 A30      .  x  .  .  .
            A3  A10 A17 A24 A31      x  x  x  .  .
            A4  A11 A18 A25 A32      .  x  .  .  .
            A5  A12 A19 A26 A33      x  x  x  .  .
            A6  A13 A20 A27 A34      .  x  .  .  x
            A7  A14 A21 A28 A35      .  .  x  x  .

   Hexwert:                         28 7C AA 82 40

   Beispiel Eurozeichen als Zeichen mit Asciicode 0x01:

   static const unsigned char eurobmp0[5] PROGMEM =
     { 0x28, 0x7c, 0xaa, 0x82, 0x40 };

   vfd_setuserchar(0x18, &eurobmp[0]);
   vfd_send(0x18);

   ------------------------------------------------------- */
void vfd_setuserchar(uint8_t nr, const uint8_t *userchar)
{
  uint8_t b;

  vfd_send(0x0a);                       // Cmd fuer user definable character
  vfd_send(nr- 0x17);                   // Position des Characters

  // Upload der 5 Characterbytes
  for (b= 0; b< 5; b++) vfd_send(pgm_read_byte(userchar++));
}


/* ----------------------------------------------------------
     gotoxy

     positioniert den (nicht sichtbaren) Cursor an die
     Koordinaten x,y. Koordinatenwert 0,0 ist links oben
   ---------------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  vfd_send(0x02);
  vfd_send((x % 20)+(y*20)+1);
}

/* ----------------------------------------------------------
     clrscr

     loescht den Displayinhalt
   ---------------------------------------------------------- */
void clrscr(void)
{
  vfd_send(0x01);
  gotoxy(0,0);
}
