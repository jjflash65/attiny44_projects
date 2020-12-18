/* ------------------------------------------------------------
                         n5110.c

   Softwaremodul fuer die Anbindung eines s/w Nokia LCD an
   einen ATtiny. Abgespeckte LCD-Bibliothek, keine Grafik-
   routinen

   17.08.2017 R. Seelig
   ---------------------------------------------------------- */

/*

                                                 ATtiny44
                                             Anschlusspins IC

                                              +-----------+
                                         Vcc  | 1      14 |  GND
                   PCINT8 - XTAL1 - CLKI PB0  | 2   A  13 |  PA0 - ADC0 - AREF - PCINT0
                        PCINT9 - XTAL2 - PB1  | 3   T  12 |  PA1 - ADC1 - AIN0 - PCINT1
                 PCINT11 - /reset - dW - PB3  | 4   t  11 |  PA2 - ADC2 - AIN1 - PCINT2
         PCINT10 - INT0 - OC0A - CKOUT - PB2  | 5   i  10 |  PA3 - ADC3 - T0 - PCINT3
            PCINT7 - ICP - OC0B - ADC7 - PA7  | 6   n   9 |  PA4 - ADC4 - USCK - SCL - T1 - PCINT4
PCINT6 - OC1A - SDA - MOSI - DI - ADC6 - PA6  | 7   y   8 |  PA5 - ADC5 - DO - MISO - OC1B PCINT5
                                              +-----------+
Hinweis:

    bei Verwendung von USI als Hardware SPI ist der sendende Ausgang
    D0. Das heisst: PA5 entspricht hier dann MOSI (im Gegensatz zur
    Verwendung von ISP bei der der Anschluss PA5 MISO ist) !!!!


    Wiring:

       ATTiny44             Display
    ---------------------------------

       PA4  (9)   ---       CLK (5)
       PA5  (8)   ---       DIN (4)
       PA0 (13)   ---       DC  (3)
       PA2 (11)   ---       CE  (2)
       PA1 (12)   ---       RST (1)


*/

#include "n5110.h"

/*

  Die Prototypen aus tx4_n5110.h

  void spi_init(void);
  unsigned char spi_out(uint8_t data);
  void wrcmd(uint8_t command);                            // sende ein Kommando
  void lcd_init();                                        // initialisiere das Display
  void wrdata(uint8_t dat);                               // sende ein Datum
  void clrscr();                                          // loesche das Display
  void gotoxy(char x,char y);                             // positioniere Ausgabeposition
  void lcd_putchar_d(char ch);                            // setze Zeichen auf das Display
  void putramstring(char *c);                             // schreibe String aus dem RAM
  void putromstring(const unsigned char *dataPtr);        // dto. ROM

  #define prints(tx)     (putromstring(PSTR(tx)))         // Anzeige String aus Flashrom: prints("Text");
  #define printa(tx)     (putramstring(tx))               // Anzeige eines Strings der in einem Array im RAM liegt
*/

char wherex= 0;
char wherey= 0;
char invchar= 0;               // = 1 fuer inversive Textausgabe

/* -------------------- SPI Interface -------------------------- */

/* -------------------------------------------------------------
   SPI_INIT

     Anschlusspins des SPI-Interface konfigurieren
   ------------------------------------------------------------- */
void spi_init(void)
{
  SPI_DDR |= (1 << SPI_MOSI_PIN) | (1 << SPI_CLK_PIN) | (1 << SPI_SS_PIN);

  SPI_DDR &= ~(1 << SPI_MISO_PIN);
  SPI_PORT |= (1 << SPI_MISO_PIN);
}


#if (hardware_spi == 1)

  /* -------------------------------------------------------------
     SPI_OUT

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

#if (hardware_spi == 0)

  /* -------------------------------------------------------------
     SPI_OUT

        Byte ueber Software SPI senden / empfangen
        data ==> zu sendendes Datum
     ------------------------------------------------------------- */
  unsigned char spi_out(uint8_t data)
  {
    char a;

    for (a=0;a<8;a++)
    {
      if((data & 0x80)> 0) { LCD_PORT |= (1 << SPI_MOSI_PIN); }
                      else { LCD_PORT &= ~(1 << SPI_MOSI_PIN); }

      LCD_PORT |= (1 << SPI_CLK_PIN);                           // Taktleitung auf 1
      LCD_PORT &= ~(1 << SPI_CLK_PIN);                          // und wieder auf 0

      data <<= 1;
    }

  }

#endif


/* -------------------------------------------------------------
   WRCMD

   sendet Kommando via SPI an das LCD
   ------------------------------------------------------------- */
void wrcmd(uint8_t cmd)
{
  LCD_PORT &= ~(1 << LCD_DC_PIN);                    // C/D = 0 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(cmd);                                      // senden
  LCD_PORT |= (1 << LCD_CE_PIN );                    // Disable Controller
}

/* -------------------------------------------------------------
   WRDATA

   sendet Datum via SPI an das LCD
   ------------------------------------------------------------- */
void wrdata(uint8_t data)
{
  LCD_PORT |= (1 << LCD_DC_PIN);                     // C/D = 1 Kommandomodus
  LCD_PORT &= ~(1 << LCD_CE_PIN);                    // Display Controller enable
  spi_out(data);                                     // senden
  LCD_PORT |= (1 <<  LCD_CE_PIN );                   // Disable Controller
}

/* -------------------------------------------------------------
   LCD_INIT

   initialisiert das Display

   Register PCD8544
   Functionset: 0x20 + DB0..DB2
   ----------------------------------------------------
                  0          |      1
     DB0: basic instruction  |   extended
     DB1: horizontal addr    |   vertical addr
     DB2: diplay active      |   powerdown


   Display-ctrl: 0x08 + DB2 + DB0
   ----------------------------------------------------
       DB2  |  DB0
        0       0    : display blank
        0       1    : all segments on
        1       0    : normal mode
        1       1    : inverted mode

   Temperature: 0x04 + DB0 + DB1
   ----------------------------------------------------
       DB0..DB1 => Temperaturecoeff 0..3


   VOP (contrast voltage  LC-Display) : 0x80 + DB0..DB5
   ----------------------------------------------------
       DB0..DB5 => voltage to the Display

   BIAS : 0x10 + DB0..DB2
   ----------------------------------------------------

   ------------------------------------------------------------- */
void lcd_init(uint8_t contrast, uint8_t bias, uint8_t temp)
{
  LCD_DDR |= (1 << LCD_RST_PIN) | (1 << LCD_DC_PIN);     // Set LCD Output pins
  LCD_PORT &= ~(1 << LCD_RST_PIN);                       // Resets LCD controler
  _delay_ms(1);
  LCD_PORT |= (1 << LCD_RST_PIN);                        // Set LCD CE = 1 (Disabled)
  LCD_PORT |= (1 << LCD_CE_PIN);

  spi_init();

  // LCD Controller Kommandos  (eigentliches initialisieren)

  contrast &= 0x7f;                       // fuer Contrast nur DB0..DB6 gueltig
  bias &= 0x07;                           // fuer bias nur DB0..DB2 gueltig
  temp &= 0x03;                           // fuer Temperaturkoeffizient nur DB0..DB1 gueltig

  wrcmd(0x21);                            // Erweiterter Kommandomodus (Register Function set: 0x20)
  wrcmd(0x80 | contrast);                 // VOP
  wrcmd(0x10 | bias);                     // Bias
  wrcmd(0x04 | temp);                     // Temp
  wrcmd(0x20);                            // Standart Kommandomodus
  wrcmd(0x0c);                            // normale Ausgabe (normal = 0Ch, invertiert = 0Dh)

  clrscr();
}


/* -----------------------------------------------------
   CLRSCR

   loescht den Displayinhalt
   ----------------------------------------------------- */
void clrscr()
{
  int  i=0;

  wrcmd(0x80);             // Anfangsadresse des Displays
  wrcmd(0x40);
  for(i=1;i<(LCD_REAL_X_RES * LCD_REAL_Y_RES/8);i++) { wrdata(0x00); }
  gotoxy(0,0);
}

/* -----------------------------------------------------
   GOTOXY

   positioniert die Textausgabeposition auf X/Y
   ----------------------------------------------------- */
void gotoxy(char x,char y)
{
 wrcmd(0x80+(x*6));
 wrcmd(0x40+y);
 wherex= x; wherey= y;
}

/* -----------------------------------------------------
   PUTCHAR_D

   gibt ein Zeichen auf dem Display aus
   (die Steuerkommandos \n und \r fuer <printf> sind
   implementiert)
   ----------------------------------------------------- */
void lcd_putchar_d(char ch)
{
  int b,rb;

  if (ch== 13)
  {
    gotoxy(0,wherey);
    return;
  }
  if (ch== 10)
  {
    wherey++;
    gotoxy(wherex,wherey);
    return;
  }

  if (ch== 8)
  {
    if ((wherex> 0))
    {
      wherex--;
      gotoxy(wherex,wherey);
      for (b= 0;b<6;b++)
      {
        if (invchar) {wrdata(0xff);} else {wrdata(0);}
      }
      gotoxy(wherex,wherey);
    }
    return;
  }

  if ((ch<0x20)||(ch>lastascii)) ch = 92;               // ASCII Zeichen umrechnen

  // Kopiere Daten eines Zeichens aus dem Zeichenarray in den LCD-Screenspeicher

  for (b= 0;b<5;b++)
  {
    rb= pgm_read_byte(&(fonttab[ch-32][b]));
    if (invchar) {rb= ~rb;}
    wrdata(rb);
  }
  if (invchar) {wrdata(0xff);} else {wrdata(0);}
  wherex++;
  if (wherex> 15)
  {
    wherex= 0;
    wherey++;
  }
  gotoxy(wherex,wherey);
}

/* ---------------------------------------------------
   PUTRAMSTRING
   gibt einen Text aus dem RAM an der aktuellen Position aus
   ---------------------------------------------------*/
void putramstring(char *c)                              // Uebergabe eines Zeigers (auf einen String)
{
  while (*c)
  {
    lcd_putchar_d(*c++);
  }
}

/* ---------------------------------------------------
   PUTROMSTRING
   gibt einen Text aus dem ROM an der aktuellen Position aus
   ---------------------------------------------------*/
void putromstring(const unsigned char *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr)) lcd_putchar_d(c);
}

