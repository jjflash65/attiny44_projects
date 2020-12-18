/* -------------------------------------------------------
                        lcd_7seg.h

     Header fuer Softwaremodul zur Anbindung eines
     zweistelligen 7-Segment LCD's OHNE eigenen Controller
     ("pures Display") ueber 2 Schieberegister SN74HC595

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     05.11.2018  R. Seelig
   ------------------------------------------------------ */


/*
                                                ATtiny44
                                              +-----------+
                                         Vcc  | 1      14 |  GND
                   PCINT8 - XTAL1 - CLKI PB0  | 2   A  13 |  PA0 - ADC0 - AREF - PCINT0
                        PCINT9 - XTAL2 - PB1  | 3   T  12 |  PA1 - ADC1 - AIN0 - PCINT1
                 PCINT11 - /reset - dW - PB3  | 4   t  11 |  PA2 - ADC2 - AIN1 - PCINT2
         PCINT10 - INT0 - OC0A - CKOUT - PB2  | 5   i  10 |  PA3 - ADC3 - T0 - PCINT3
            PCINT7 - ICP - OC0B - ADC7 - PA7  | 6   n   9 |  PA4 - ADC4 - USCK - SCL - T1 - PCINT4
PCINT6 - OC1A - SDA - MOSI - DI - ADC6 - PA6  | 7   y   8 |  PA5 - ADC5 - DO - MISO - OC1B PCINT5
                                              +-----------+



                      SN74HC595
                     +----------+
                  Q1 | 1     16 | Vcc
                  Q2 | 2     15 | Q0
                  Q3 | 3     14 | ser_data
                  Q4 | 4     13 | /oe
                  Q5 | 5     12 | strobe (clk_latch)
                  Q6 | 6     11 | clk_ser_data
                  Q7 | 7     10 | /master_clr
                 GND | 8      9 | Q7S (register overflow)
                     +----------+



            LCD 2-Digit 7-Segmentanzeige

         bp  1g  1f  1a  1b  2g  2f  2a  2b
          _   _   _   _   _   _   _   _   _
         | | | | | | | | | | | | | | | | | |
       +-------------------------------------+
       |            _a_       _a_            |
       |          f|   |b   f|   |b          |
       |           |_g_|     |_g_|           |
       |          e|   |c   e|   |c          |
       |        .  |___|  .  |___|           |
       |             d         d             |
       +-------------------------------------+
         |_| |_| |_| |_| |_| |_| |_| |_| |_|
         bp  dp1 1e  1d  1c  dp2 2e  2d  2c


     --------------------------------------------
                      Verdrahtung
     --------------------------------------------


             ATtiny44            SN74HC595 (1)                   SN74HC595 (2)
         ----------------------------------------------------------------------
                                 16 Vcc                     ...  16 Vcc

             12 (PA1)    ...     14 (seriell data)
                                 13 (/oe) = GND             ...  13 (/oe) = GND
             11 (PA2)    ...     12 (strobe)                ...  12 (strobe)
             13 (PA0)    ...     11 (clock)                 ...  11 (clock)
                                 10 (/master reset) = Vcc   ...  10 (/master reset) = Vcc
                                  9 Q7S                     ...  14 (seriell data)
                                  8 GND                     ...   8 GND


   Segmentbelegung der Anzeige:

       a
      ---
   f | g | b            Segment 2 (Lo)        | dp |  g  |  f  |  e  |  d  |  c  |  b  |  a  |
      ---               ----------------------------------------------------------------------
   e |   | c            Bit-Nr. SN74HC595 (1) |  7 |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
      ---
       d
                        Segment 1 (Hi)        | bp |  g  |  f  |  e  |  d  |  c  |  b  |  a  |
                        ----------------------------------------------------------------------
                        Bit-Nr. SN74HC595 (2) |  7 |  6  |  5  |  4  |  3  |  2  |  1  |  0  |

   Segmente werden bei einer logischen 1 angezeigt

*/

#ifndef in_lcd_7seg
  #define in_lcd_7seg

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>

  #include "avr_gpio.h"


  //  Anschlusspins des SN74HC595

  #define srclock_init()      PA0_output_init()
  #define srclock_set()       PA0_set()
  #define srclock_clr()       PA0_clr()

  #define srdata_init()       PA1_output_init()
  #define srdata_set()        PA1_set()
  #define srdata_clr()        PA1_clr()

  #define srstrobe_init()     PA2_output_init()
  #define srstrobe_set()      PA2_set()
  #define srstrobe_clr()      PA2_clr()

  #define sr_init()           { srclock_init(); srdata_init(); srstrobe_init(); }


  extern uint8_t lcd7s_buffer;
  extern uint8_t lcd7s_dp;

  /* ----------------------------------------------------------
                            Prototypen
     ---------------------------------------------------------- */

  void lcd7s_dezout(uint8_t value);
  void lcd7s_hexout(uint8_t value);
  void lcd7s_init(void);

#endif