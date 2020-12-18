/* -----------------------------------------------------
                        oled1306_spi.h

    Header fuer das Anbinden eines OLED Displays mit
    SSD1306 Controller und SPI Interface

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


    Pinbelegung:

       ATTiny44             Display
    ---------------------------------

                            GND              (1)
                            Vcc              (2)
       PA4  (9)   ---       D0 (CLK)         (3)
       PA5  (8)   ---       D1 (DIN / MOSI)) (4)
       PA1 (12)   ---       RST              (5)
       PA7  (6)   ---       DC               (6)
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

#ifndef in_ssd1306spi
  #define in_ssd1306spi

  #include <util/delay.h>
  #include <util/atomic.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include "avr_gpio.h"

  #define hardware_spi          1        // 0 : SPI ueber Bitbanging realisieren, hier
                                         //     duerfen dann die Anschlusspins veraendert werden
                                         // 1 : SPI ueber USI-Hardware realisieren,
                                         //     Anschlusspins fuer Mosi, Miso, Clk duerfen
                                         //     NICHT veraendert werden


  #define sw_mosiinit()         PA5_output_init()
  #define sw_csinit()           PA2_output_init()
  #define sw_resinit()          PA1_output_init()
  #define sw_dcinit()           PA0_output_init()
  #define sw_sckinit()          PA4_output_init()

  #define dc_set()              ( PA0_set() )
  #define dc_clr()              ( PA0_clr() )
  #define ce_set()              ( PA2_set() )
  #define ce_clr()              ( PA2_clr() )
  #define rst_set()             ( PA1_set() )
  #define rst_clr()             ( PA1_clr() )

  #define mosi_set()            ( PA5_set() )
  #define mosi_clr()            ( PA5_clr() )
  #define sck_set()             ( PA4_set() )
  #define sck_clr()             ( PA4_clr() )


  #define oled_enable()         ( PA2_clr() )
  #define oled_disable()        ( PA2_set() )
  #define oled_cmdmode()        ( PA0_clr() )      // SPI-Wert als Kommando
  #define oled_datamode()       ( PA0_set() )      // SPI-Wert als Datum

  #define delay     _delay_ms

  extern uint8_t aktxp;
  extern uint8_t aktyp;
  extern uint8_t doublechar;
  extern uint8_t bkcolor;
  extern uint8_t textcolor;


  void ssd1306_init(void);
  void gotoxy(uint8_t x, uint8_t y);
  void clrscr(void);
  void oled_putchar(uint8_t ch);

#endif

