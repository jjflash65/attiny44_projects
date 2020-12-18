/* -----------------------------------------------------
                        oled1306_spi.c

    Softwaremodul fuer das Anbinden eines OLED Displays
    mit SSD1306 Controller und SPI Interface

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


     12.09.2018 R. Seelig

  ------------------------------------------------------ */

/* ----------------------------------------------------
    Pinbelegung::

       ATTiny44             Display
    ---------------------------------

                            GND              (1)
                            Vcc              (2)
       PA4  (9)   ---       D0 (CLK)         (3)
       PA5  (8)   ---       D1 (DIN / MOSI)) (4)
       PA1 (12)   ---       RST              (5)
       PA0 (13)   ---       DC               (6)
       PA2 (11)   ---       CE               (7)


                     G   V           R
                     N   c   D   D   E   D   C
                     D   c   0   1   S   C   s
                 +-------------------------------+
                 |   o   o   o   o   o   o   o   |
                 |                               |
                 |   -------------------------   |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |  |                         |  |
                 |   -----+-------------+-----   |
                 |        |             |        |
                 |        +-------------+        |
                 +-------------------------------+

*/

#include "oled1306_spi.h"
#include "font8x8h.h"


/*  ---------------------------------------------------------
                       globale Variable
    --------------------------------------------------------- */

uint8_t aktxp= 0;
uint8_t aktyp= 0;
uint8_t doublechar;
uint8_t bkcolor= 0;
uint8_t textcolor= 1;

/* -------------------------------------------------------------
                           spi_init

     Anschlusspins des SPI-Interface konfigurieren
   ------------------------------------------------------------- */
void spi_init(void)
{
  sw_mosiinit();
  sw_csinit();
  sw_resinit();
  sw_dcinit();
  sw_sckinit();
}


#if (hardware_spi == 0)

  /* -----------------------------------------------------
                             spi_out

          Byte ueber Software SPI senden / empfangen
          data ==> zu sendendes Datum
     ----------------------------------------------------- */
  void spi_out(uint8_t value)
  {
    char a;

    for (a= 0; a< 8; a++)
    {
      if((value & 0x80)> 0) { mosi_set(); }
                       else { mosi_clr(); }
      // Taktimpuls erzeugen
      sck_set();
      sck_clr();
      value= value << 1;
    }
  }
#endif

#if (hardware_spi == 1)

  /* -------------------------------------------------------------
                             spi_out

        Byte ueber USI Hardware senden / empfangen
        data ==> zu sendendes Datum
     ------------------------------------------------------------- */
  unsigned char spi_out(uint8_t data)
  {
    volatile uint8_t r;

    USIDR = data;
    USISR = (1 << USIOIF);

    while ( (USISR & (1 << USIOIF)) == 0 )
    {
      USICR = (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 << USITC);
    }
    return USIDR;
  }
#endif


/*  ---------------------------------------------------------
                        ssd1306_init
              initialisiert das OLED-Display
    --------------------------------------------------------- */
void ssd1306_init(void)
{
  spi_init();

  oled_enable();
  delay(10);

  rst_clr();                    // Display-reset
  delay(10);
  rst_set();

  oled_cmdmode();               // nachfolgende Daten als Kommando

  spi_out(0x8d);                // Ladungspumpe an
  spi_out(0x14);

  spi_out(0xaf);                // Display on
  delay(150);

  spi_out(0xa1);                // Segment Map
  spi_out(0xc0);                // Direction Map
  oled_datamode();
}

/*  ---------------------------------------------------------
                       oled_setxybyte

      setzt ein Byte an Koordinate x,y

      Anmerkung:
            da Display monochrom werden (leider) immer
            8 Pixel in Y Richtung geschrieben, daher ist
            Wertebereich fuer y = 0..7 !

            Bsp. Koordinate y== 6 beschreibt tatsaechliche
            y-Koordinaten 48-55 (inclusive)
    --------------------------------------------------------- */
void oled_setxybyte(uint8_t x, uint8_t y, uint8_t value)
{
    oled_cmdmode();
    y= 7-y;

    spi_out(0xb0 | (y & 0x0f));
    spi_out(0x10 | (x >> 4 & 0x0f));
    spi_out(x & 0x0f);

    oled_datamode();
    if ((!textcolor)) value= ~value;
    spi_out(value);
}

/*  ---------------------------------------------------------
                           clrscr

      loescht den Displayinhalt mit der in bkcolor ange-
      gebenen "Farbe" (0 = schwarz, 1 = hell)
    --------------------------------------------------------- */

void clrscr(void)
{
  uint8_t x,y;

  oled_enable();

  rst_clr();                    // Display-reset
  delay(1);
  rst_set();

  oled_cmdmode();               // nachfolgende Daten als Kommando

  spi_out(0x8d);                // Ladungspumpe an
  spi_out(0x14);

  spi_out(0xaf);                // Display on

  spi_out(0xa1);                // Segment Map
  spi_out(0xc0);                // Direction Map
  oled_datamode();


  for (y= 0; y< 8; y++)         // ein Byte in Y-Achse = 8 Pixel...
                                // 8*8Pixel = 64 Y-Reihen
  {
    oled_cmdmode();

    spi_out(0xb0 | y);          // Pageadresse schreiben
    spi_out(0x00);              // MSB X-Adresse
    spi_out(0x00);              // LSB X-Adresse (+Offset)

    oled_datamode();
    for (x= 0; x< 128; x++)
    {
      if (bkcolor) spi_out(0xff); else spi_out(0x00);
    }
  }

}

/*  ---------------------------------------------------------
                     oled_setpageadr

      adressiert den Speicher des Displays (und gibt somit
      die Speicherstelle an, die als naechstes beschrieben
      wird)
    --------------------------------------------------------- */
void oled_setpageadr(uint8_t x, uint8_t y)
{
  oled_cmdmode();
  y= 7-y;

  spi_out(0xb0 | (y & 0x0f));
  spi_out(0x10 | (x >> 4 & 0x0f));
  spi_out(x & 0x0f);

  oled_datamode();
}

/*  ---------------------------------------------------------
                            gotoxy

       legt die naechste Textausgabeposition auf dem
       Display fest. Koordinaten 0,0 bezeichnet linke obere
       Position
    --------------------------------------------------------- */
void gotoxy(uint8_t x, uint8_t y)
{
  aktxp= x;
  aktyp= y;
  x *= 8;
  y= 7-y;
  oled_cmdmode();
  spi_out(0xb0 | (y & 0x0f));
  spi_out(0x10 | (x >> 4 & 0x0f));
  spi_out(x & 0x0f);
  oled_datamode();
}

/*  ---------------------------------------------------------
                         oled_putchar

       gibt ein Zeichen auf dem Display aus. Steuerzeichen
       (fuer bspw. printf) sind implementiert:

               13 = carriage return
               10 = line feed
                8 = delete last char
    --------------------------------------------------------- */
void oled_putchar(uint8_t ch)
{
  uint8_t  i, b;
  uint8_t  z1;
  uint16_t z2[8];
  uint16_t z;

  if (ch== 13)                                          // Fuer <printf> "/r" Implementation
  {
    aktxp= 0;
    gotoxy(aktxp, aktyp);
    return;
  }
  if (ch== 10)                                          // fuer <printf> "/n" Implementation
  {
    aktyp++;
    gotoxy(aktxp, aktyp);
    return;
  }

  if (ch== 8)
  {
    if ((aktxp> 0))
    {

      aktxp--;
      gotoxy(aktxp, aktyp);

      for (i= 0; i< 8; i++)
      {
       if ((!textcolor)) spi_out(0xff); else spi_out(0x00);
      }
      gotoxy(aktxp, aktyp);
    }
    return;

  }

  if (doublechar)
  {
    for (i= 0; i< 8; i++)
    {
      // Zeichen auf ein 16x16 Zeichen vergroessern
      z1= pgm_read_byte(&(font8x8h[ch-' '][i]));
      z2[i]= 0;
      for (b= 0; b< 8; b++)
      {
        if (z1 & (1 << b))
        {
          z2[i] |= (1 << (b*2));
          z2[i] |= (1 << ((b*2)+1));
        }
      }
    }

    for (i= 0; i< 8; i++)
    {
      z= z2[i];
      if ((!textcolor)) z= ~z;
      z= z >> 8;
      spi_out(z);
      spi_out(z);
    }
    gotoxy(aktxp, aktyp+1);
    for (i= 0; i< 8; i++)
    {
      z= z2[i];
      if ((!textcolor)) z= ~z;
      z= z & 0xff;
      spi_out(z);
      spi_out(z);
    }
    aktyp--;
    aktxp +=2;
    if (aktxp> 15)
    {
      aktxp= 0;
      aktyp +=2;
    }
    gotoxy(aktxp,aktyp);
  }
  else
  {
    for (i= 0; i< 8; i++)
    {
      if ((!textcolor)) spi_out(~(pgm_read_byte(&(font8x8h[ch-' '][i]))));
                   else spi_out(pgm_read_byte(&(font8x8h[ch-' '][i])));
    }
    aktxp++;
    if (aktxp> 15)
    {
      aktxp= 0;
      aktyp++;
    }
    gotoxy(aktxp,aktyp);
  }
}

/*  ---------------------------------------------------------
                         reversebyte

       spiegelt die Bits eines Bytes. D0 tauscht mit D7
       die Position, D1 mit D6 etc.
    --------------------------------------------------------- */
uint8_t reversebyte(uint8_t value)
{
  uint8_t hb, b;

  hb= 0;
  for (b= 0; b< 8; b++)
  {
    if (value & (1 << b)) hb |= (1 << (7-b));
  }
  return hb;
}

