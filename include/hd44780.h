/*   -------------------------------------------------------
                          hd44780.h

     Header fuer grundlegende Funktionen eines HD44780
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

#ifndef in_txlcd
  #define in_txlcd

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <stdint.h>

  #include "avr_gpio.h"


  /* -------------------------------------------------------
        Pinbelegung
     ------------------------------------------------------- */

  #define rs_init()      PA6_output_init()           // command / data
  #define e_init()       PA5_output_init()           // clock

  #define d4_init()      PA4_output_init()
  #define d5_init()      PA2_output_init()
  #define d6_init()      PA1_output_init()
  #define d7_init()      PA0_output_init()

  #define rs_set()       PA6_set()
  #define rs_clr()       PA6_clr()

  #define e_set()        PA5_set()
  #define e_clr()        PA5_clr()

  #define d4_set()       PA4_set()
  #define d4_clr()       PA4_clr()

  #define d5_set()       PA2_set()
  #define d5_clr()       PA2_clr()

  #define d6_set()       PA1_set()
  #define d6_clr()       PA1_clr()

  #define d7_set()       PA0_set()
  #define d7_clr()       PA0_clr()


  #define txlcd_prints(tx)      (txlcd_putromstring(PSTR(tx)))       // Anzeige String aus Flashrom: prints("Text");

  /* -------------------------------------------------------
       diverse Macros
     ------------------------------------------------------- */

  #define testbit(reg,pos) ((reg) & (1<<pos))               // testet an der Bitposition pos das Bit auf 1 oder 0


  /* -------------------------------------------------------
       Prototypen
     ------------------------------------------------------- */

    void txlcd_init(void);
    void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar);
    void gotoxy(uint8_t x, uint8_t y);
    void txlcd_putchar(char ch);
    void txlcd_putramstring(uint8_t *c);
    void txlcd_putromstring(const uint8_t *dataPtr);

    extern uint8_t wherex,wherey;

#endif
