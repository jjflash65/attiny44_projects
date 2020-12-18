/* -----------------------------------------------------------------------------------
                         tftdisplay.h

     Header zum Softwaremodul fuer farbige TFT-Displays. Aufgrund der
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

     17.09.2018  R. Seelig   ----------------------------------------------------------------------------------- */

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


#ifndef in_tftdisplay_module
  #define in_tftdisplay_module

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>

  #include "avr_gpio.h"

  // Auswahl des Displaycontrollers (ILI9163 und S6D02A1 haben dieselben Sequenzen)

  #define  ili9163                  0
  #define  ili9340                  0
  #define  st7735r                  1
  #define  s6d02a1                  0
  #define  ili9225                  0

  /*  ------------------------------------------------------------
                         Displayaufloesung
      ------------------------------------------------------------ */

  // fuer Berechnung Bildadressen. ACHTUNG: manche Chinadisplays behandeln 128x128 Displays
  // so, als haette es 160 Pixel in Y-Aufloesung.

  // In diesem Fall ist fuer _lcyofs  -32 anzugeben
  // (hat nur Effekt, wenn _yres   128 , im Hauptprogramm dann outmode= 3; damit das Bild
  // nicht af dem Kopf steht)

  #define tft128                  1                 // 1: Display ohne Offset (aelter)
                                                    // 2: Display mit Offset (neuer)

  #define rgbseq                  1                 // Reihenfolge der erwarteten Farbuebergabe bei ST7735 LC-Controllern
                                                    // 0: blau-gruen-rot
                                                    // 1: rot-gruen-blau

  #define _xres                   128
  #define _yres                   128

  #if (tft128 == 2)
    #define _lcyofs               -32               // manche Display sprechen das Display an
                                                    // als haette es 160 Pixel Y-Aufloesung
  #else
    #define _lcyofs               0
  #endif

  #define tft_wait                0                 // 0 = keine Wartefunktion nach spi_out
                                                    // 1 = es wird nach spi_out ein nop eingefuegt

  #define mirror                  0                 // 0 : normale Ausgabe
                                                    // 1 : Spiegelbildausgabe

  #define hardware_spi            1                 // 0 : SPI ueber Bitbanging realisieren, hier
                                                    //     duerfen dann die Anschlusspins veraendert werden
                                                    // 1 : SPI ueber USI-Hardware realisieren,
                                                    //     Anschlusspins fuer Mosi, Miso, Clk duerfen
                                                    //     NICHT veraendert werden

/*  ------------------------------------------------------------
                       Pinbelegung
    ------------------------------------------------------------

       ATTiny44             Display
    ---------------------------------

                            LED (1) +3.3V
       PA4  (9)   ---       CLK (2)
       PA5  (8)   ---       DIN (3)
       PA0 (13)   ---       DC  (4)
       PA1 (12)   ---       RST (5)
       PA2 (11)   ---       CE  (6)
                            GND (7)
                            Vcc (8)
*/

  #define mosi_init()             PA5_output_init()
  #define clk_init()              PA4_output_init()

  #define mosi_set()              PA5_set()
  #define mosi_clr()              PA5_clr()
  #define clk_set()               PA4_set()
  #define clk_clr()               PA4_clr()

  #define rst_init()              PA1_output_init()
  #define ce_init()               PA2_output_init()
  #define dc_init()               PA0_output_init()

  #define rst_set()               PA1_set()
  #define rst_clr()               PA1_clr()
  #define ce_set()                PA2_set()
  #define ce_clr()                PA2_clr()
  #define dc_set()                PA0_set()
  #define dc_clr()                PA0_clr()


  #define lcd_pin_init()          { rst_init(); ce_init(); dc_init(); }


  #define lastascii 126                       // letztes angegebenes Asciizeichen

  /*  ------------------------------------------------------------
                         P R O T O T Y P E N
      ------------------------------------------------------------ */

  // ----------------- LCD - Benutzerfunktionen ---------------

  void lcd_init(void);                                                        // initialisiert Display
  void putpixel(int x, int y,uint16_t color);                                 // schreibt einen einzelnen Punkt auf das Display
  void clrscr();                                                              // loescht Display-Inhalt
  uint16_t rgbfromvalue(uint8_t r, uint8_t g, uint8_t b);                     // konvertiert einen 24 Bit RGB-Farbwert in einen 16 Bit Farbwert
  uint16_t rgbfromega(uint8_t entry);                                         // konvertiert einen Farbwert aus der EGA-Palette in einen 16 Bit Farbwert
  void gotoxy(unsigned char x, unsigned char y);                              // setzt den Textcursor fuer Textausgaben
  void lcd_putchar(uint8_t ch);                                               // setzt ein Zeichen auf das Display
  void lcd_putramstring(char *c);                                                 // Uebergabe eines Zeigers (auf einen String)
  void lcd_putromstring(const unsigned char *dataPtr);

  /*  ------------------------------------------------------------
                      EGA - Farbzuweisungen
      ------------------------------------------------------------ */

  #define black                   0
  #define blue                    1
  #define green                   2
  #define cyan                    3
  #define red                     4
  #define magenta                 5
  #define brown                   6
  #define grey                    7
  #define darkgrey                8
  #define lightblue               9
  #define lightgreen              10
  #define lightcyan               11
  #define lightred                12
  #define lightmagenta            13
  #define yellow                  14
  #define white                   15

  //-------------------------------------------------------------
  // Registerzuordnung der Adressierungsregister der
  // verschiedenen Displaycontroller
  //-------------------------------------------------------------


  #if  (ili9225 == 1)
    #define coladdr      0x20
    #define rowaddr      0x21
    #define writereg     0x22
  #else
    #define coladdr      0x2a
    #define rowaddr      0x2b
    #define writereg     0x2c
  #endif

  //-------------------------------------------------------------
  //  Variable Farben
  //-------------------------------------------------------------

  extern uint16_t textcolor;        // Beinhaltet die Farbwahl fuer die Vordergrundfarbe
  extern uint16_t bkcolor;          // dto. fuer die Hintergrundfarbe
  extern uint16_t egapalette [];    // Farbwerte der DOS EGA/VGA Farben


  //-------------------------------------------------------------
  //  Variable und Defines Schriftzeichen
  //-------------------------------------------------------------

  extern int aktxp;                 // Beinhaltet die aktuelle Position des Textcursors in X-Achse
  extern int aktyp;                 // dto. fuer die Y-Achse
  extern uint8_t outmode;           // Richtungssinn der Displayausgabe
  extern uint8_t textsize;          // Skalierung der Ausgabeschriftgroesse
  extern uint8_t fntfilled;         // gibt an, ob eine Zeichenausgabe ueber einen Hintergrund gelegt
                                    // wird, oder ob es mit der Hintergrundfarbe aufgefuellt wird
                                    // 1 = Hintergrundfarbe wird gesetzt, 0 = es wird nur das Fontbitmap
                                    // gesetzt, der Hintergrund wird belassen

#endif
