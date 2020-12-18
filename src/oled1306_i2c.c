/* -----------------------------------------------------
                        oled1306.c

    Softwaremodul fuer das Anbinden eines OLED Displays
    mit SSD1306 Controller und I2C Interface

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

#include "oled1306_i2c.h"
#include "font8x8h.h"


/* -------------------------------------------------------
                        globale Variable
   ------------------------------------------------------- */
uint8_t aktxp= 0;                               // aktuelle X-Cursorposition
uint8_t aktyp= 0;                               // dto. y
uint8_t doublechar= 0;                          // Flag fuer Zeichenausgabe in gross
uint8_t bkcolor= 0;                             // Hintergrundfarbe (0 = schwarz)
uint8_t textcolor= 1;                           // Textfarbe (1 = weiss)


/* -------------------------------------------------------
                   ssd1306_writecmd

     sendet ein Kommandobyte an das Display
   ------------------------------------------------------- */
void ssd1306_writecmd(uint8_t cmd)
{
  i2c_start(ssd1306_addr);
  i2c_write(0x00);
  i2c_write(cmd);
  i2c_stop();
}

/* -------------------------------------------------------
                   ssd1306_writedata

    sendet ein Datenbyte an das Display
   ------------------------------------------------------- */
void ssd1306_writedata(uint8_t data)
{
  i2c_start(ssd1306_addr);
  i2c_write(0x40);
  i2c_write(data);
  i2c_stop();
}

/* -------------------------------------------------------
                     ssd1306_init

    Initialisiert das Display fuer den Gebrauch
   ------------------------------------------------------- */
void ssd1306_init(void)
{
  i2c_master_init();
  //Init LCD

  ssd1306_writecmd(0x8d);      // Ladungspumpe an
  ssd1306_writecmd(0x14);
  ssd1306_writecmd(0xaf);      // Display an
  _delay_ms(150);
  ssd1306_writecmd(0xa1);      // Segment Map
  ssd1306_writecmd(0xc0);      // Direction Map
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

  i2c_start(ssd1306_addr);
  i2c_write(0x00);

  i2c_write(0xb0 | (y & 0x0f));
  i2c_write(0x10 | (x >> 4 & 0x0f));
  i2c_write(x & 0x0f);

  i2c_stop();
}


/*  ---------------------------------------------------------
                           clrscr

      loescht den Displayinhalt mit der in bkcolor ange-
      gebenen "Farbe" (0 = schwarz, 1 = hell)
    --------------------------------------------------------- */
void clrscr(void)
{
  uint8_t x,y;

  i2c_start(ssd1306_addr);
  i2c_write(0x00);

  i2c_write(0x8d);                  // Ladungspumpe an
  i2c_write(0x14);

  i2c_write(0xaf);                  // Display on

  i2c_write(0xa1);                  // Segment Map
  i2c_write(0xc0);                  // Direction Map

  i2c_stop();

  for (y= 0; y< 8; y++)                  // ein Byte in Y-Achse = 8 Pixel...
                                         // 8*8Pixel = 64 Y-Reihen
  {
    i2c_start(ssd1306_addr);
    i2c_write(0x00);

    i2c_write(0xb0 | y);            // Pageadresse schreiben
    i2c_write(0x00);                // MSB X-Adresse
    i2c_write(0x00);                // LSB X-Adresse (+Offset)

    i2c_stop();

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (x= 0; x< 128; x++)
    {

      if (bkcolor) i2c_write(0xff); else i2c_write(0x00);

    }
    i2c_stop();

  }
  gotoxy(0,0);
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

  if (ch== 0) return;

  if (ch== 13)                                          // Fuer <printf> "/r" Implementation
  {
    aktxp= 0;
    gotoxy(aktxp, aktyp);
    return;
  }
  if (ch== 10)                                          // fuer <printf> "/n" Implementation
  {
    aktyp++;
    if (doublechar) aktyp++;
    gotoxy(aktxp, aktyp);
    return;
  }

  if (ch== 8)
  {
    if ((aktxp> 0))
    {

      aktxp--;
      gotoxy(aktxp, aktyp);

      i2c_start(ssd1306_addr);
      i2c_write(0x40);
      for (i= 0; i< 8; i++)
      {
       if ((!textcolor)) i2c_write(0xff); else i2c_write(0x00);
      }
      i2c_stop();
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

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 8; i++)
    {
      z= z2[i];
      if ((!textcolor)) z= ~z;
      z= z >> 8;
      i2c_write(z);
      i2c_write(z);
    }
    i2c_stop();
    gotoxy(aktxp, aktyp+1);

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 8; i++)
    {
      z= z2[i];
      if ((!textcolor)) z= ~z;
      z= z & 0xff;
      i2c_write(z);
      i2c_write(z);
    }
    i2c_stop();

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

    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 8; i++)
    {
      if ((!textcolor)) i2c_write(~(pgm_read_byte(&(font8x8h[ch-' '][i]))));
                   else i2c_write(pgm_read_byte(&(font8x8h[ch-' '][i])));
    }
    i2c_stop();
    aktxp++;
    if (aktxp> 15)
    {
      aktxp= 0;
      aktyp++;
    }
    gotoxy(aktxp,aktyp);
  }
}

