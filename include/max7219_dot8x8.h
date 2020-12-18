/* -----------------------------------------------------------
                         max7219_dot8x8.c

   Header zum Softwaremodul fuer eine  Ansteuerung des
   Multpilex IC MAX7219 in Verbindung mit einer 8x8 LED Matrix
   (China-Modul)


   Benoetigte Hardware;

            - 8x8 LED Matrix mit MAX7219 IC

   MCU   :  ATtiny44
   Takt  :  extern oder extern


   24.09.2018 by R. Seelig
   ----------------------------------------------------------- */

/*
            Verbindung MAX7219 Matrixmodul zu ATtiny44:

                              +-----------------+
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              | o o o o o o o o |
                              |                 |
                              +-----------------+
                              |+---------------+|
                              ||    MAX7219    ||
                              |+---------------+|
                              +- |  |  |  |  | -+
                                 |  |  |  |  |

                                 V  G  D  C  C
                                 c  N  i  S  L
                                 c  D  n     K



       MCU Attiny44           8x8 LED-Matrixmodul
       Funktion ( Pin )             Funktion
       ------------------------------------------
          PA0 (  )    ...........      Din
          PA2 (  )    ...........      SCK
          PA1 (  )    ...........      CS

Anmerkung: CS ist eine irrefuehrende Bezeichnung, da es der Uebernahmeimpuls
           eingegangener Daten an die Matrix ist. Innerhalb der Software wird
           deshalb der Name <m7219_load> verwendet.
*/

#ifndef in_max7219
  #define in_max7219

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>

  #include "avr_gpio.h"
  #include "font8x8h.h"


  // Pinzuordnung und Ein- Ausschaltmakros
  #define m7219_dininit()    PA0_output_init()
  #define m7219_dinset()     PA0_set()
  #define m7219_dinclr()     PA0_clr()

  #define m7219_clkinit()    PA2_output_init()
  #define m7219_clkset()     PA2_set()
  #define m7219_clkclr()     PA2_clr()

  #define m7219_loadinit()   PA1_output_init()
  #define m7219_loadset()    PA1_set()
  #define m7219_loadclr()    PA1_clr()


  // Pins als Ausgaenge setzen
  #define m7219_init_con()   { m7219_dininit();                      \
                               m7219_clkinit();                      \
                               m7219_loadinit(); }

  // "Framebuffer" der ein komplettes 8x8 Pixelbitmap aufnehmen kann
  extern uint8_t fbuf[8];

/* -----------------------------------------------------------
                          PROTOTYPEN
   ----------------------------------------------------------- */

  // MAX7219 Funktionen
  void m7219_init();
  void m7219_clrscr();
  void m7219_col(uint8_t digit, uint8_t value);
  void m7219_setbmp(uint8_t *bmp);
  void m7219_setpgmbmp(const uint8_t *bmp);

  // Framebuffer Funktionen

  void fbuf_clr(uint8_t *dest);
  void fbuf_putpixel(uint8_t *dest, uint8_t x, uint8_t y, uint8_t c);
  void fbuf_shl_ins(uint8_t *dest, uint8_t src);
  void fbuf_scroll_in(uint8_t *dest, uint8_t *src, int dtime);
  void fbuf_scroll_text_in(uint8_t *dest, uint8_t *txt, int dtime1, int dtime2);


#endif
