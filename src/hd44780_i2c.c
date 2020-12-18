/* ------------------------------------------------------------------
                              hd44780_i2c.c

     Softwaremodul fuer die Anbindung eines HS44780 kompatiblen
     Textdisplays ueber einen PCF8574 I2C I/O Expander

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Benoetigt i2c_sw.c


     Pinbelegung I2C
     ---------------
     PB0 = SDA
     PB1 = SCL


     23.10.2018 R. Seelig
   ------------------------------------------------------------------ */

#include "hd44780_i2c.h"

uint8_t pcf_value = 0;    // beinhaltet das zuletzt ueber I2C zuletzt ausgegebene
                          // Nibble sodass mittels Bitmanipulation die rs- bzw. die clk
                          // Leitung aktiviert werden koennen

char wherex, wherey;


/* #################################################################
     Funktionen PCF8574 I/O-Expander
   ################################################################# */

void pcf8574_write(uint8_t value)
{
  i2c_start(pcf8574_addr);
  i2c_write(value);
  i2c_stop();
}


/* -------------------------------------------------------
     NIBBLEOUT

     sendet ein Halbbyte am Ausgang des Schieberegsters

         WERT= gesamtes Byte
         HILO= 1 => oberen 4 Bits werden ausgegeben
         HILO= 0 => untere 4 Bits werden ausgegeben
   ------------------------------------------------------- */
void nibbleout(unsigned char wert, unsigned char hilo)
{

  pcf_value &= 0x0f;
  if (hilo)
  {
    pcf_value |= (wert & 0xf0);
  }
  else
  {
    pcf_value |= (wert << 4);
  }
  pcf8574_write(pcf_value);
}

/* -------------------------------------------------------
      txlcd_takt

      gibt einen Clockimpuls an das Display
   ------------------------------------------------------- */

void txlcd_takt(void)
{
  clk_set();
  clk_clr();
}

/* -------------------------------------------------------
      txlcd_io

      sendet ein Byte an das Display an die in den
      Defines angegebenen Pins
              Wert = zu sendendes Byte
   ------------------------------------------------------- */

void txlcd_io(char wert)
{
  nibbleout(wert,1);
  txlcd_takt();
  nibbleout(wert,0);
  txlcd_takt();
}

/* -------------------------------------------------------
     txlcd_init

     bereitet das Display fuer den Betrieb vor.
     Es wird 4-Bit Datenuebertragung verwendet.

   ------------------------------------------------------- */

void txlcd_init(void)
{
  char i;

  pcf_value= 0;

  /* ##########################################################
       Warum auch immer:
       zuerst einen 8-Bit Datentransfer setzen (bei direkt an-
       geschlossenen Pins nicht notwendig) um danach den
       (richtigen) 4-Bit Datentransfermodus zu starten
     ########################################################## */
  for (i= 0; i< 3; i++)
  {
    nibbleout(0x30,1);
    txlcd_takt();
    _delay_ms(8);
  }

  /* ##########################################################
       4-Bit Datentransfermodus, 2 Zeilen
     ########################################################## */
  for (i= 0; i< 3; i++)
  {
    nibbleout(0x20,1);
    txlcd_takt();
    _delay_ms(8);
  }

  txlcd_io(0x28);                     // 4 Bit, 2 Lines
  _delay_ms(8);

  txlcd_io(0x0c);                     // Display an, Cursor aus
  _delay_ms(8);

  txlcd_io(0x01);                     // Display clear
  _delay_ms(8);

  wherex= 0; wherey= 0;
}

/* -------------------------------------------------------
     gotoxy

     setzt den Textcursor an eine Stelle im Display. Die
     obere linke Ecke hat die Koordinate (1,1)
   ------------------------------------------------------- */
void gotoxy(char x, char y)
{
  unsigned char txlcd_adr, b;

  y--;
  x--;
  txlcd_adr= 0x80 + (( (y & 0x01) * 0x40)  + x);

  // fuer 4 zeilige Displays
  b= (y & 0x02);                     // 3. oder 4 Zeile angewaehlt
  txlcd_adr |= b << 3;               // ... und Zeichenadressoffset berechnen

  rs_clr();
  txlcd_io(txlcd_adr);
  wherex= x+1;
  wherey= y+1;
}

/* -------------------------------------------------------
     clrscr

     loescht das Display und setzt den Cursor auf Position
     1,1
   ------------------------------------------------------- */
void clrscr(void)
{
  rs_clr();
  txlcd_io(0x01);       // Display loeschen
  gotoxy(1,1);
}


/* -------------------------------------------------------
     txlcd_putchar

     platziert ein Zeichen auf dem Display.

               ch = auszugebendes Zeichen
   ------------------------------------------------------- */
void txlcd_putchar(char ch)
{
  rs_set();
  txlcd_io(ch);
  wherex++;
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


