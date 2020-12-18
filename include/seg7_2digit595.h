/* ----------------------------------------------------------
                        seg7_2digit595.h


     Header fuer Softwaremodul zur Ansteuerung einer
     2-stelligen 7-Segmentanzeige mit gemeinsamer Kathode
     ueber ein einzelnes SN74HC595  Schieberegister

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     24.10.2018  R. Seelig


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


                              2-Digit 7-Segmentanzeige 7 gemeinsame Kathode
                                            f  GK1  b   a  GK2
                                            _   _   _   _   _
                                           | | | | | | | | | |
                                         +---------------------+
                                         q    _a_       _a_    q
                                         q  f|   |b   f|   |b  q
                                         q   |_g_|     |_g_|   q
                                         q  e|   |c   e|   |c  q
                                         q   |___| .   |___| . q
                                         q     d         d     q
                                         +---------------------+
                                           |_| |_| |_| |_| |_|
                                            e   d   c   g   dp


               Pinbelegungen

   ATtiny44            SN74HC595
   -------------------------------------
                       16 Vcc
                       15 Q0
   11 (PA2)    ...     14 (seriell data)
                       13 (/oe) = GND
   10 (PA3)    ...     12 (strobe)
   12 (PA1)    ...     11 (clock)
                       10 (/master reset) = Vcc
                        9 (seriell out; Q7S)
                        8 GND


   SN74HC595           2-Digit 7-Segmentanzeige
   --------------------------------------------
   15 (Q0)     ...     seg_a
    1 (Q1)     ...     seg_b
    2 (Q2)     ...     seg_c
    3 (Q3)     ...     seg_d
    4 (Q4)     ...     seg_e
    5 (Q5)     ...     seg_f
    6 (Q6)     ...     seg_g
    7 (Q7)     ...     mpx

    Hinweis: an die mpx-Leitung muss ein NPN-Transistorschalter in
             Emitterschaltung angeschlossen werden, um ein /mpx
             zu generieren

*/

#ifndef in_seg7_2digit
  #define in_seg7_2digit

  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include "avr_gpio.h"


  extern uint8_t    digit2_outmode;             // 0 : Ziffernausgabe
                                                // 1 : Ansteuerung einzelner Segmente (mit Bitmuster in
                                                //     digit2_segvalues)

  extern uint16_t   digit2_segvalues;           // beinhaltet bei Segmentausgabe das auszugebende Bitmuster


  //  Anschlusspins des Moduls
  #define srdata_init()       PA2_output_init()
  #define srdata_set()        PA2_set()
  #define srdata_clr()        PA2_clr()

  #define srstrobe_init()     PA3_output_init()
  #define srstrobe_set()      PA3_set()
  #define srstrobe_clr()      PA3_clr()

  #define srclock_init()      PA1_output_init()
  #define srclock_set()       PA1_set()
  #define srclock_clr()       PA1_clr()

  /* ----------------------------------------------------------
                             Prototypen
     ---------------------------------------------------------- */

  void digit2_init(void);
  void digit2_dezout(uint8_t value);
  void digit2_hexout(uint8_t value);
  void timer1_init(void);

#endif
