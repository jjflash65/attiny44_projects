/*   -------------------------------------------------------
                          hd44780.c

     Softwaremodul fuer grundlegende Funktionen eines HD44780
     kompatiblen LC-Textdisplays

     Hardware : Text-LCD

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     17.09.2018  R. Seelig

   ------------------------------------------------------- */

/*
      Anschluss am Bsp. Pollin-Display C0802-04 an
      einen ATtiny44
      Pinkonfiguration fuer - lcdpindef1 -
      ---------------------------------------------------
         o +5V
         |                            Display                 Controller
         _                        Funktion   PIN            PIN    Funktion
        | |
        |_|                          GND      1 ------------
         |                          +5V       2 ------------
         o----o Kontrast   ---    Kontrast    3 ------------
        _|_                           RS      4 ------------    7    PA6
        \ /                          GND      5 ------------
        -'-                    (Takt) E       6 ------------    8    PA5
         |                           D4       7 ------------    9    PA4
        --- GND                      D5       8 ------------   11    PA2
                                     D6       9 ------------   12    PA1
                                     D7      10 ------------   13    PA0

*/

#include "hd44780.h"

uint8_t wherex, wherey;


/* -------------------------------------------------------
     nibbleout

     sendet ein Halbbyte an das LC-Display

     Uebergabe:
         value  : gesamtes Byte
         nibble : 1 => HiByte wird gesendet
                  0 => LoByte wird gesendet
         HILO= 1 => oberen 4 Bits werden gesendet
         HILO= 0 => untere 4 Bits werden gesendet
   ------------------------------------------------------- */
void nibbleout(uint8_t value, uint8_t nibble)
{
/*
  if (nibble == 1) nibble = 0x04;      // Maske Hi-Lo-Byte, ergaenzt Werte zu 4..7
              else nibble = 0x00;      // :-) oder eben nicht (lo-Byte)

    if (testbit(value, (uint8_t)(3 | nibble) )) d7_set(); else d7_clr();
    if (testbit(value, (uint8_t)(2 | nibble) )) d6_set(); else d6_clr();
    if (testbit(value, (uint8_t)(1 | nibble) )) d5_set(); else d5_clr();
    if (testbit(value, (uint8_t)(0 | nibble) )) d4_set(); else d4_clr();
*/

  // belegt 30 Bytes weniger Flashspeicher
  if (nibble)
  {
    if (testbit(value, 7 )) d7_set(); else d7_clr();
    if (testbit(value, 6 )) d6_set(); else d6_clr();
    if (testbit(value, 5 )) d5_set(); else d5_clr();
    if (testbit(value, 4 )) d4_set(); else d4_clr();
  }
  else
  {
    if (testbit(value, 3 )) d7_set(); else d7_clr();
    if (testbit(value, 2 )) d6_set(); else d6_clr();
    if (testbit(value, 1 )) d5_set(); else d5_clr();
    if (testbit(value, 0 )) d4_set(); else d4_clr();
  }

}


/*
void nibbleout(unsigned char wert, unsigned char hilo)
{
  if (hilo)
  {
     if (testbit(wert,7)) { d7_set(); }
                    else  { d7_clr(); }
     if (testbit(wert,6)) { d6_set(); }
                    else  { d6_clr(); }
     if (testbit(wert,5)) { d5_set(); }
                    else  { d5_clr(); }
     if (testbit(wert,4)) { d4_set(); }
                    else  { d4_clr(); }
  }
  else
  {
     if (testbit(wert,3)) { d7_set(); }
                    else  { d7_clr(); }
     if (testbit(wert,2)) { d6_set(); }
                    else  { d6_clr(); }
     if (testbit(wert,1)) { d5_set(); }
                    else  { d5_clr(); }
     if (testbit(wert,0)) { d4_set(); }
                    else  { d4_clr(); }
  }
}
*/

/* -------------------------------------------------------
      txlcd_clock

      gibt einen Clockimpuls an das Display
   ------------------------------------------------------- */
void txlcd_clock(void)
{
  e_set();
  _delay_us(60);
  e_clr();
  _delay_us(60);
}

/* -------------------------------------------------------
      txlcd_io

      sendet ein Byte an das Display

      Uebergabe:
         value = zu sendender Wert
   ------------------------------------------------------- */
void txlcd_io(uint8_t value)
{
  nibbleout(value, 1);
  txlcd_clock();
  nibbleout(value, 0);
  txlcd_clock();
}

/* -------------------------------------------------------
     txlcd_init

     initialisiert das Display im 4-Bitmodus
   ------------------------------------------------------- */
void txlcd_init(void)
{
  char i;

  d4_init(); d5_init(); d6_init(); d7_init();
  rs_init(); e_init();

  rs_clr();
  for (i= 0; i< 3; i++)
  {
    txlcd_io(0x20);
    _delay_ms(6);
  }
  txlcd_io(0x28);
  _delay_ms(6);
  txlcd_io(0x0c);
  _delay_ms(6);
  txlcd_io(0x01);
  _delay_ms(6);
  wherex= 0; wherey= 0;
}

/* -------------------------------------------------------
     gotoxy

     setzt den Textcursor an eine Stelle im Display. Die
     obere linke Ecke hat die Koordinate (1,1)
   ------------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  uint8_t txlcd_adr;

  txlcd_adr= (0x80+((y-1)*0x40))+x-1;
  rs_clr();
  txlcd_io(txlcd_adr);
  wherex= x;
  wherey= y;
}

/* -------------------------------------------------------
     txlcd_setuserchar

     kopiert die Bitmap eines benutzerdefiniertes Zeichen
     in den Charactergenerator des Displaycontrollers

               nr : Position im Ram des Displays, an
                    der die Bitmap hinterlegt werden
                    soll.
        *userchar : Zeiger auf die Bitmap des Zeichens

   Bsp.:  txlcd_setuserchar(3,&meinezeichen[0]);
          txlcd_putchar(3);

   ------------------------------------------------------- */
void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar)
{
  uint8_t b;

  rs_clr();
  txlcd_io(0x40+(nr << 3));                         // CG-Ram Adresse fuer eigenes Zeichen
  rs_set();
  for (b= 0; b< 8; b++) txlcd_io(pgm_read_byte(userchar++));
  rs_clr();
}


/* -------------------------------------------------------
     txlcd_putchar

     gibt ein Zeichen auf dem Display aus

     Uebergabe:
         ch = auszugebendes Zeichen
   ------------------------------------------------------- */

void txlcd_putchar(char ch)
{
  rs_set();
  txlcd_io(ch);
  wherex++;
}

/* -------------------------------------------------------
      txlcd_putramstring

      gibt einen AsciiZ Text der im RAM gespeichert ist
      auf dem Display aus.

      Bsp.:

      char strbuf[] = "H. Welt";

      putramstring(strbuf);
   ------------------------------------------------------- */

void txlcd_putramstring(uint8_t *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    txlcd_putchar(*c++);
  }
}

/* -------------------------------------------------------
     txlcd_putromstring

     gibt einen Text, der im Flashrom gespeichert ist
     auf dem LC-Display aus (Wird vom Macro <prints>
     aufgerufen .

     Bsp.:

       static const uint8_t mytext[] PROGMEM = "Text";

       putromstring(PSTR("H. Welt"));
       putromstring(&mytext[0]);

       prints("Hallo");

   ------------------------------------------------------- */
void txlcd_putromstring(const uint8_t *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr))
    txlcd_putchar(c);
}
