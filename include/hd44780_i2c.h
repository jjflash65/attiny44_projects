/* ------------------------------------------------------------------
                              hd44780_i2c.c

     Header fuer Anbindung eines HS44780 kompatiblen Textdisplays
     ueber einen PCF8574 I2C I/O Expander

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

/* ------------------------------------------------------------------
                        Anschlussbelegung:

             Text-Display                 PCF8574
            Funktion   PIN              PIN    Funktion

              GND       1
              +5V       2
         Kontrast       3
               RS       4 ------------    7    P0
        R/W (GND)       5
         (Takt) E       6 ------------    8    P2
               D4      11 ------------    9    P3
               D5      12 ------------   10    P4
               D6      13 ------------   11    P6
               D7      14 ------------   12    P7

   ------------------------------------------------------------------*/


#ifndef in_txlcd_i2c
  #define in_txlcd_i2c

  #include <util/delay.h>
  #include <avr/pgmspace.h>

  #include "i2c_sw.h"

  #define pcf8574_addr  0x40       // I2C-Adresse Portexpander


  #define lcd_rs        0          // Bitposition RS-Anschluss am 8574 - Portexpander
  #define lcd_clk       2          // Bitposition E-Anschluss am 8574 -Portexpander


  #define clk_set()     { pcf_value |= 1 << lcd_clk;  pcf8574_write(pcf_value); }
  #define clk_clr()     { pcf_value &= ~(1 << lcd_clk);  pcf8574_write(pcf_value); }

  #define rs_set()      { pcf_value |= 1 << lcd_rs; pcf8574_write(pcf_value); }
  #define rs_clr()      { pcf_value &= ~(1 << lcd_rs); pcf8574_write(pcf_value); }

  extern char wherex, wherey;      // beinhaltet die aktuelle Position des Cursors

  /* #################################################################
       Prototypen
     ################################################################# */

  void pcf8574_write(uint8_t value);

  void txlcd_init(void);
  void gotoxy(char x, char y);
  void clrscr(void);
  void txlcd_putchar(char ch);
  void txlcd_setuserchar(uint8_t nr, const uint8_t *userchar);

#endif