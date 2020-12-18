/* ------------------------------------------------------------

                        tx4_n5110.h

   Headerdatei fuer die Anbindung eines s/w Nokia LCD an
   einen ATtiny. Abgespeckte LCD-Bibliothek, keine Grafik-
   funktionen

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

#ifndef in_tx4_n5110
  #define in_tx4_n5110

  #include <util/delay.h>
  #include <util/atomic.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include "font5x7.h"


  #define hardware_spi                 1        // 0 : SPI ueber Bitbanging realisieren, hier
                                                //     duerfen dann die Anschlusspins veraendert werden
                                                // 1 : SPI ueber USI-Hardware realisieren,
                                                //     Anschlusspins fuer Mosi, Miso, Clk duerfen
                                                //     NICHT veraendert werden

  #define SPI_DDR                 DDRA
  #define SPI_PORT                PORTA
  #define SPI_MOSI_PIN            PA5
  #define SPI_MISO_PIN            PA6
  #define SPI_CLK_PIN             PA4
  #define SPI_SS_PIN              PA2

  #define LCD_PORT                PORTA
  #define LCD_DDR                 DDRA
  #define LCD_RST_PIN             PA1
  #define LCD_DC_PIN              PA0
  #define LCD_CE_PIN              SPI_SS_PIN


  #ifdef n3410
    // Nokia 3410

    #define LCD_VISIBLE_X_RES     96
    #define LCD_VISIBLE_Y_RES     64
    #define LCD_REAL_X_RES        102
    #define LCD_REAL_Y_RES        72
    #define OK                    0
    #define OUT_OF_BORDER         1

  #else
    // Nokia 5110 / 3310

    #define LCD_VISIBLE_X_RES     84
    #define LCD_VISIBLE_Y_RES     48
    #define LCD_REAL_X_RES        84
    #define LCD_REAL_Y_RES        48
    #define OK                    0
    #define OUT_OF_BORDER         1
  #endif

  /* ----------------------------------------------------
                          Prototypen
     ---------------------------------------------------- */

  void spi_init(void);
  unsigned char spi_out(uint8_t data);
  void wrcmd(uint8_t command);                                  // sende ein Kommando
  void lcd_init(uint8_t contrast, uint8_t bias, uint8_t temp);  // initialisiert das Display
  void wrdata(uint8_t dat);                                     // sende ein Datum
  void clrscr();                                                // loesche das Display
  void gotoxy(char x,char y);                                   // positioniere Ausgabeposition
  void lcd_putchar_d(char ch);                                  // setze Zeichen auf das Display
  void putramstring(char *c);                                   // schreibe String aus dem RAM
  void putromstring(const unsigned char *dataPtr);              // dto. ROM

  #define prints(tx)     (putromstring(PSTR(tx)))               // Anzeige String aus Flashrom: prints("Text");
  #define printa(tx)     (putramstring(tx))                     // Anzeige eines Strings der in einem Array im RAM liegt


  /* ----------------------------------------------------
     Variable der Displayroutinen
     ---------------------------------------------------- */

  extern char wherex;
  extern char wherey;
  extern char invchar;               // = 1 fuer inversive Textausgabe

#endif

