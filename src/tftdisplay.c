/* -------------------------------------------------------
                         tftdisplay.c

     Softwaremodul fuer farbige TFT-Displays. Aufgrund der
     sehr "sparsamen" Bestueckung des ATtiny nur als
     Textdisplay genutzt

     unterstuetzte Displaycontroller:
           ili9163
           ili9340
           st7735r
           s6d02a1
           ili9225

     MCU   :  ATtiny44
     Takt  :  interner oder externer Takt

     17.09.2018  R. Seelig
   ----------------------------------------------------------------------------------- */

#include "tftdisplay.h"
#include "font8x8.h"

/* -----------------------------------------------------------------------------------
      Displays aus China werden haeufig mit unterschiedlichen
      Bezeichnungen der Pins ausgeliefert. Moegliche
      Pinzuordnungen sind:

       Controller ATmega           Display
      --------------------------------------------------------------------------
         SPI-SCK  / P      ----    SCK / CLK    (clock)
         SPI-MOSI / P      ----    SDA / DIN    (data in display)
         SPI-SS   / P      ----    CS  / CE     (chip select display)
                    P      ----    A0  / D/C    (selector data or command write)
                    P      ----    Reset / RST  (reset)

      siehe auch verschiedene vordefinierte pindefs
   ------------------------------------------------------------------------------------ */


  /*  ------------------------------------------------------------
                         P R O T O T Y P E N
                           aus tftdisplay.h
      ------------------------------------------------------------ */


/*  ------------------------------------------------------------
                             globales
    ------------------------------------------------------------ */

#define delay       _delay_ms

// ------------------------------------
//          Variable Farben
// ------------------------------------

// RGB565 Farbpallete der "EGA"-Farben
uint16_t egapalette [] =
    { 0x0000, 0x0015, 0x0540, 0x0555,
      0xa800, 0xa815, 0xaaa0, 0xad55,
      0x52aa, 0x52bf, 0x57ea, 0x57ff,
      0xfaaa, 0xfabf, 0xffea, 0xffff };


// ------------------------------------
//            Variable Text
// ------------------------------------

int      aktxp;             // Beinhaltet die aktuelle Position des Textcursors in X-Achse
int      aktyp;             // dto. fuer die Y-Achse
uint16_t textcolor;         // Beinhaltet die Farbwahl fuer die Vordergrundfarbe
uint16_t bkcolor;           // dto. fuer die Hintergrundfarbe
uint8_t  outmode;
uint8_t  textsize;          // Skalierung der Ausgabeschriftgroesse

uint8_t  fntfilled = 1;         // gibt an, ob eine Zeichenausgabe ueber einen Hintergrund gelegt

  //-------------------------------------------------------------
  //  Displayinitialisierungsequenzen
  //-------------------------------------------------------------

  #define delay_flag 0x80                                // Markierungsflag: bestimmt, ob nach einem Kommando

  #if ( (s6d02a1 == 1) || (ili9163 == 1) )

    static const uint8_t PROGMEM lcdinit_seq[] =         // Initialisierungssequenzen
    {
      29,                                                // Anzahl Gesamtkommandos

    /*
      Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
      CMD    | Anzahl Datas | Datas | evtl. Delaytime
    */

      0xf0, 2,0x5a,0x5a,
      0xfc, 2,0x5a,0x5a,                                                                   // Excommand3
      0x26, 1,0x01,                                                                        // Gammaset
      0xfa, 15,0x02,0x1f,0x00,0x10,0x22,0x30,0x38,0x3A,0x3A,0x3A,0x3A,0x3A,0x3d,0x02,0x01, // Positivegammacontrol
      0xfb, 15,0x21,0x00,0x02,0x04,0x07,0x0a,0x0b,0x0c,0x0c,0x16,0x1e,0x30,0x3f,0x01,0x02, // Negativegammacontrol
      0xfd, 11,0x00,0x00,0x00,0x17,0x10,0x00,0x01,0x01,0x00,0x1f,0x1f,                     // Analogparametercontrol
      0xf4, 15,0x00,0x00,0x00,0x00,0x00,0x3f,0x3f,0x07,0x00,0x3C,0x36,0x00,0x3C,0x36,0x00, // Powercontrol
      0xf5, 13,0x00,0x70,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6d,0x66,0x06,           // VCOMcontrol
      0xf6, 11,0x02,0x00,0x3f,0x00,0x00,0x00,0x02,0x00,0x06,0x01,0x00,                     // Sourcecontrol
                                                                                           // Displaycontrol
      0xf2, 17,0x00,0x01,0x03,0x08,0x08,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x04,0x08,0x08,
      0xf8, 1, 0x11,                                                                       // Gatecontrol
      0xf7, 4,0xc8,0x20,0x00,0x00,                                                         //Interfacecontrol
      0xf3, 2,0x00,0x00,                                                                   //Powersequencecontrol
      0x11, delay_flag,50,                                                                 //Wake
      0xf3, 2+delay_flag,0x00,0x01,50,                                                     //Powersequencecontrol
      0xf3, 2+delay_flag,0x00,0x03,50,
      0xf3, 2+delay_flag,0x00,0x07,50,
      0xf3, 2+delay_flag,0x00,0x0f,50,
      0xf4, 15+delay_flag,0x00,0x04,0x00,0x00,0x00,0x3f,0x3f,0x07,0x00,0x3C,0x36,0x00,0x3C,0x36,0x00,50,
      0xf3, 2+delay_flag,0x00,0x1f,50,
      0xf3, 2+delay_flag,0x00,0x7f,50,
      0xf3, 2+delay_flag,0x00,0xff,50,
      0xfd, 11,0x00,0x00,0x00,0x17,0x10,0x00,0x00,0x01,0x00,0x16,0x16,                     // Analogparametercontrol
      0xf4, 15,0x00,0x09,0x00,0x00,0x00,0x3f,0x3f,0x07,0x00,0x3C,0x36,0x00,0x3C,0x36,0x00,
      0x36, 1,0x08,                                                                        //Memoryaccessdatacontrol
      0x35, 1,0x00,                                                                        //Tearingeffectlineon
      0x3a, 1+delay_flag,0x05,150,                                                         //Interfacepixelcontrol

      0x29,0,                                                                              //Displayon
      0x2c,0                                                                               //Memorywrite
    };
  #endif

  #if (st7735r == 1)

    static const uint8_t PROGMEM lcdinit_seq[] =            // Initialisierungssequenzen
    {
        21,                                                 // Anzahl Gesamtkommandos

        0x01, delay_flag,150,
        0x11, delay_flag,255,
        0xb1, 3, 0x01, 0x2C, 0x2D,
        0xb2, 3, 0x01, 0x2C, 0x2D,
        0xb3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
        0xb4, 1, 0x07,
        0xc0, 3, 0xA2, 0x02, 0x84,
        0xc1, 1, 0xC5,
        0xc2, 2, 0x0A, 0x00,
        0xc3, 2, 0x8A, 0x2A,
        0xc4, 2, 0x8A, 0xEE,
        0xc5, 1, 0x0E,
        0x20, 0,

        #if ( rgbseq == 0 )
          0x36, 1, 0xc0,                                      // 00 fuer auf dem Kopf stehend
                                                              // C0 fuer Normal, BGR Farbfolge
                                                              // C8 fuer Normal, RGB Farbfolge
        #endif

        #if ( rgbseq == 1 )
          0x36, 1, 0xc8,                                      // 00 fuer auf dem Kopf stehend
                                                              // C0 fuer Normal, BGR Farbfolge
                                                              // C8 fuer Normal, RGB Farbfolge
        #endif

        0x3a, 1+delay_flag, 0x05,10,

        0x2a, 4, 0x00, 0x00, 0x00, 0x7F,
        0x2b, 4, 0x00, 0x00, 0x00, 0x9F,

        0xe0, 16, 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
                  0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
        0xe1, 16, 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
                  0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
        0x13, delay_flag,10,
        0x29, delay_flag,100,
    };
  #endif


  #if (ili9340 == 1)
    static const uint8_t PROGMEM lcdinit_seq[] =              // Initialisierungssequenzen
    {
      22,
                                                              // Anzahl Gesamtkommandos
    //
    //  Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
    //  CMD    | Anzahl Datas | Datas | evtl. Delaytime
    //


      0x01,     delay_flag, 200,
      0xcb, 5,  0x39, 0x2c, 0x00, 0x34, 0x02,
      0xcf, 3,  0x00, 0xc1, 0x30,
      0xe8, 3,  0x85, 0x00, 0x78,
      0xea, 2,  0x00, 0x00,
      0xed, 4,  0x64, 0x03, 0x12, 0x81,
      0xf7, 1,  0x20,
      0xc0, 1,  0x23,                                // power control => VRH[5:0]
      0xc1, 1,  0x10,                                // power control => SAP[2:0]; BT[3:0]
      0xc5, 2,  0x3e, 0x28,                          // VCM control
      0xc7, 1,  0x86,                                // VCM control2
      0x36, 1,  0x48,                                // memory access controll
      0x3a, 1,  0x55,
      0xb1, 2,  0x00, 0x18,
      0xb6, 3,  0x08, 0x82, 0x27,                    // display control
      0xf2, 1,  0x00,                                // gamma function
      0x26, 1,  0x01,                                // gamma curve selected

      // set gama
      0xe0, 15, 0x0f, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00,
      0xe1, 15, 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0f,

      0x11,     delay_flag, 130,
      0x29,     delay_flag, 10,
      0x2c,     delay_flag, 10
    };
  #endif

  #if (ili9225 == 1)
    static const uint8_t PROGMEM lcdinit_seq[] =      // Initialisierungssequenzen
    {
      34,                                             // Anzahl Gesamtkommandos

    /*
      Byte 0 | Byte 1       | Byte 2 u. folgende | evtl. Delaytime-Byte
      CMD    | Anzahl Datas | Datas | evtl. Delaytime
    */

      0x01, 2+delay_flag, 0x01,0x1c,50,
      0x02, 2, 0x01,0x00,
      0x03, 2, 0x10,0x38,
      0x08, 2, 0x08,0x08,

      0x0c, 2, 0x00,0x00,
      0x0f, 2, 0x0e,0x01,
      0x20, 2, 0x00,0x00,
      0x21, 2+delay_flag, 0x00,0x00,50,

      //power ON sequence
      0x10, 2, 0x09,0x00,
      0x11, 2+delay_flag,0x10,0x38,50,

      0x12, 2, 0x11,0x21,
      0x13, 2, 0x00,0x65,
      0x14, 2, 0x50,0x58,

      //set GRAM area
      0x30, 2, 0x00,0x00,
      0x31, 2, 0x00,0xdb,
      0x32, 2, 0x00,0x00,
      0x33, 2, 0x00,0x00,
      0x34, 2, 0x00,0xdb,
      0x35, 2, 0x00,0x00,
      0x36, 2, 0x00,0xaf,
      0x37, 2, 0x00,0x00,
      0x38, 2, 0x00,0xdb,
      0x39, 2, 0x00,0x00,

      //adjust the gamma curve
      0x50, 2, 0x04,0x00,
      0x51, 2, 0x06,0x0b,
      0x52, 2, 0x0c,0x0a,
      0x53, 2, 0x01,0x05,
      0x54, 2, 0x0a,0x0c,
      0x55, 2, 0x0b,0x06,
      0x56, 2, 0x00,0x04,
      0x57, 2, 0x05,0x01,
      0x58, 2, 0x0e,0x00,
      0x59, 2+delay_flag, 0x00,0x0e,50,

      0x07, 2,0x10,0x17

    };
  #endif



/* -------------------------------------------------------------
   SPI_INIT

     Anschlusspins des SPI-Interface konfigurieren
   ------------------------------------------------------------- */
void spi_init(void)
{
  mosi_init();
  clk_init();
  ce_init();
}


#if (hardware_spi == 1)

  /* -------------------------------------------------------------
     SPI_OUT

        Byte ueber USI Hardware senden / empfangen
        data ==> zu sendendes Datum
     ------------------------------------------------------------- */
  uint8_t spi_out(uint8_t data)
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

#if (hardware_spi == 0)

  /* -------------------------------------------------------------
     SPI_OUT

        Byte ueber Software SPI senden / empfangen
        data ==> zu sendendes Datum
     ------------------------------------------------------------- */
  uint8_t spi_out(uint8_t data)
  {
    char a;

    for (a=0;a<8;a++)
    {
      if((data & 0x80)> 0) mosi_set(); else mosi_clr();

      clk_set();                          // Taktleitung auf 1
      clk_clr();                          // und wieder auf 0

      data <<= 1;
    }

  }

#endif


/* -------------------------------------------------------------
   spi_lcdout

    Byte ueber SPI senden

      data : zu sendendes Datum
   ------------------------------------------------------------- */
void spi_lcdout(uint8_t data)
{
  spi_out(data);
  #if ( (ili9225 ==1) | (tft_wait == 1))
    __asm volatile
    (
      "nop\n\r"
    );
  #endif
}

/* -------------------------------------------------------------
     wrcmd

     sendet Kommando via SPI an das LCD

      data : zu sendendes Datum
   ------------------------------------------------------------- */
void wrcmd(uint8_t cmd)
{
  dc_clr();                             // C/D = 0 Kommandomodus
  spi_lcdout(cmd);                         // senden

  #if (ili9225 == 1)
    ce_set();
    ce_clr();
  #endif

}

/* -------------------------------------------------------------
     wrdata

     sendet Datum via SPI an das LCD

       data : zu sendendes Datum
   ------------------------------------------------------------- */
void wrdata(uint8_t data)
{
  dc_set();                             // C/D = 1 Kommandomodus
  spi_lcdout(data);                        // senden/
}

/* ----------------------------------------------------------
     wrdata16

     sendet ein 16 Bit Integer via SPI an das Display

       data : zu sendendes 16-Bit Datum

   ---------------------------------------------------------- */

void wrdata16(int data)
{
    int data1 = data>>8;
    int data2 = data&0xff;

    dc_set();
    spi_lcdout(data1);
    spi_lcdout(data2);
}

/* -------------------------------------------------------------
    lcd_init

    initialisiert das Display
   ------------------------------------------------------------- */
void lcd_init()
{
  volatile uint8_t  cmd_anz;
  volatile uint8_t  arg_anz;
  volatile uint16_t ms;
  uint16_t i;

  const uint8_t *tabseq;

  spi_init();
  lcd_pin_init();

  rst_clr();                            // Resets LCD controler
  delay(2);
  rst_set();

  outmode= 0;

  tabseq= &lcdinit_seq[0];

  // ein einzelnes Kommando besteht aus mehreren Datenbytes. Zuerst wird ein Kommandobyte
  // auf dem SPI geschickt, anschliessend die zu diesem Kommandobytes dazugehoerigen Datenbytes
  // abschliessend wird evtl. ein Timingwait eingefuegt. Dieses wird fuer alle vorhandenen
  // Kommandos durchgefuehrt

  cmd_anz = pgm_read_byte(tabseq++);               // Anzahl Gesamtkommandos

  while(cmd_anz--)                                 // alle Kommandos auf SPI schicken
  {
    wrcmd(pgm_read_byte(tabseq++));                // Kommando lesen
    arg_anz= pgm_read_byte(tabseq++);              // Anzahl zugehoeriger Datenbytes lesen
    ms= arg_anz & delay_flag;                      // bei gesetztem Flag folgt ein Timingbyte
    arg_anz &= ~delay_flag;                        // delay_flag aus Anzahl Argumenten loeschen
    while(arg_anz--)                               // jedes Datenbyte des Kommandos
    {
      wrdata(pgm_read_byte(tabseq++));             // senden
    }
    if(ms)                                         // wenn eine Timingangabe vorhanden ist
    {
      ms= pgm_read_byte(tabseq++);                 // Timingzeit lesen
      if(ms == 255) ms = 500;
      for (i= 0; i< ms; i++) delay(1);             // und entsprechend "nichts" tun
    }
  }
  ce_clr();
}


/* ----------------------------------------------------------
     set_ram_address

     legt den Zeichenbereich des Displays fest
   ---------------------------------------------------------- */
void set_ram_address (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  wrcmd(coladdr);
  wrdata(x1 >> 8);
  wrdata(x1);
  wrdata(x2 >> 8);
  wrdata(x2);

  wrcmd(rowaddr);
  #if (_yres == 128)
     wrdata(0x00);
     wrdata(y1+32+_lcyofs);
//     wrdata(0x00);
//     wrdata(y2+32);
     y2 += 32;					// Dummy um Warning85 zu verhindern
  #else
    wrdata(y1 >> 8);
    wrdata(y1);
    wrdata(y2 >> 8);
    wrdata(y2);
  #endif

  wrcmd(writereg);
}

/* ----------------------------------------------------------
     setcol

     schreibt einen Farbpunkt ins Display-Ram

       coladdr  : Adresse im Display-Ram
   ---------------------------------------------------------- */
void setcol(int startcol)
{
    wrcmd(coladdr);
    wrdata16(startcol);
}

/* ----------------------------------------------------------
     setpage
   ---------------------------------------------------------- */
void setpage(int startpage)
{
    wrcmd(rowaddr);
    wrdata16(startpage);
}

/* ----------------------------------------------------------
     setxypos

     waehlt die Position im Display-Ram aus, auf die der
     naechste Datentransfer erfolgen wird

       x,y : fuer diese Koordinaten wird die Speicheradresse
             im Display-Ram berechnet
   ---------------------------------------------------------- */
void setxypos(int x, int y)
{
      #if (mirror == 1)
        setcol(_xres-x);
      #else
        setcol(x);
      #endif


      #if ( _yres==128 )
        setpage(y+32+_lcyofs);
      #else
        setpage(y);
      #endif

    wrcmd(writereg);
}

/* ----------------------------------------------------------
     putpixel

     zeichnet einen einzelnen Punkt auf dem Display an der
     Koordinate x,y mit der Farbe color.

     Putpixel beruecksichtigt die globale Variable "outmode"
     mithilfe derer es moeglich ist, eine Ausgabe auf dem
     Display zu "drehen"

       x,y   : Koordinaten, an die ein Farbpixel gezeichnet
               wird
       color : RGB565 Farbwert, der gezeichnet wird
   ---------------------------------------------------------- */
void putpixel(int x, int y,uint16_t color)
{
    switch (outmode)
    {
      case 0  :  setxypos(x,y); break;
      case 1  :  setxypos(y,_yres-1-x); break;
      case 2  :  setxypos(_xres-1-y,x); break;
      case 3  :  setxypos(_xres-1-x, _yres-1-y); break;

      default : break;

    }

    wrdata16(color);
}


/* ----------------------------------------------------------
     clrscr

     loescht den Displayinhalt mit der in der Variable
     "bkcolor" angegebenen Farbe
   ---------------------------------------------------------- */

void clrscr()
{
  int      x,y;
  uint8_t  colouthi, coloutlo;

  set_ram_address(0,0,_xres-1,_yres-1);

  colouthi = bkcolor >> 8;
  coloutlo = bkcolor & 0xff;

  dc_set();

  for (y= 0; y< _yres; y++)
  {
    for (x= 0; x< _xres; x++)
    {
      wrdata(colouthi);
      wrdata(coloutlo);
    }
  }

}

/* ----------------------------------------------------------
   rgbfromvalue

     Setzt einen 16-Bitfarbwert aus 3 einzelnen Farbwerten
     fuer (r)ot, (g)ruen und (b)lau zusammen.

       r,g,b  : 8-Bit Farbwerte fuer rot, gruen, blau. Aus
                diesen wird ein 16 Bit (RGB565) Farbwert
                generiert und dieser als Funktionsergebnis
                zurueck geliefert
   ---------------------------------------------------------- */
uint16_t rgbfromvalue(uint8_t r, uint8_t g, uint8_t b)
{
  uint16_t value;

  r= r >> 3;
  g= g >> 2;
  b= b >> 3;
  value= b;
  value |= (g << 5);
  value |= (r << 11);
  return value;
}

/* ----------------------------------------------------------
     rgbfromega

     liefert den 16-Bit Farbwert, der in der Ega-Farbpalette
     definiert ist.

         entry : Indexnummer der Farbe in egapalette
   ---------------------------------------------------------- */

uint16_t rgbfromega(uint8_t entry)
{
  return egapalette[entry];
}

/* --------------------------------------------------
     gotoxy

     Setzt den Textcursor (NICHT Pixelkoordinate) an
     die angegebene Textkoordinate.

     Parameter:
        x = X-Koordinate
        y = Y-Koordinate
   -------------------------------------------------- */
void gotoxy(unsigned char x, unsigned char y)
{
  aktxp= x*(fontsizex+(textsize*fontsizex));
  aktyp= y*(fontsizey+(textsize*fontsizey));
}


/* --------------------------------------------------
     lcd_putchar

     gibt das in ch angegebene Zeichen auf dem
     Display mit einem 8x8 grossen Font aus

     Parameter:
        ch :    auszugebendes Zeichen
   -------------------------------------------------- */
void lcd_putchar(uint8_t ch)
{
  uint8_t   i,i2;
  uint8_t   b;
  int       oldx,oldy;
  uint16_t  fontint;
  uint16_t  fmask;

  if (ch== 13)                                          // Fuer <printf> "/r" Implementation
  {
    aktxp= 0;
    return;
  }
  if (ch== 10)                                          // fuer <printf> "/n" Implementation
  {
    aktyp= aktyp+fontsizey+(fontsizey*textsize);
    return;
  }

  fmask= 1<<(fontsizex-1);

  oldx= aktxp;
  oldy= aktyp;
  for (i=0; i<fontsizey; i++)
  {
    b= pgm_read_byte(&(font8x8[(ch-32)][i]));
    fontint= b;

    for (i2= 0; i2<fontsizex; i2++)
    {
      if (fmask & fontint)
      {
        putpixel(oldx,oldy,textcolor);
        if ((textsize==1))
        {
          putpixel(oldx+1,oldy,textcolor);
          putpixel(oldx,oldy+1,textcolor);
          putpixel(oldx+1,oldy+1,textcolor);
        }
      }
      else
      {
        if (fntfilled)
        {
          putpixel(oldx,oldy,bkcolor);
          if ((textsize==1))
          {
            putpixel(oldx+1,oldy,bkcolor);
            putpixel(oldx,oldy+1,bkcolor);
            putpixel(oldx+1,oldy+1,bkcolor);
          }
        }
      }
      fontint= fontint<<1;
      oldx= oldx+1+textsize;
    }
    oldy++;
    if ((textsize==1)) {oldy++; }
    oldx= aktxp;
  }
  aktxp= aktxp+fontsizex+(fontsizex*textsize);
}

/* ----------------------------------------------------------
   lcd_putramstring

   gibt einen Text an der aktuellen Position aus

      c : Zeiger auf den AsciiZ - STring
   ----------------------------------------------------------*/
void lcd_putramstring(char *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    lcd_putchar(*c++);
  }
}

/* ---------------------------------------------------
   lcd_putromstring

   gibt einen Text aus dem Flash an der aktuellen
   Position aus
   ---------------------------------------------------*/
void lcd_putromstring(const unsigned char *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) lcd_putchar(c);
}

