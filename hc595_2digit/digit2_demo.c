/* ----------------------------------------------------------
                          digit2_demo.c


     Ansteuerung einer 2-stelligen 7-Segmentanzeige mit
     gemeinsamer Kathode ueber ein einzelnes SN74HC595
     Schieberegister

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

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "seg7_2digit595.h"

#define delay    _delay_ms

/* ----------------------------------------------------------
                             M-A-I-N
   ---------------------------------------------------------- */


int main(void)
{
  uint8_t i, i2;
  uint8_t b1, b2;

  timer1_init();
  digit2_init();

  digit2_hexout(0xec);
  delay(1500);

  while(1)
  {
    digit2_outmode= 0;
    for (i= 11; i!= 0; i--)
    {
      digit2_dezout(i-1);
      delay(300);
    }
    delay(1000);

    digit2_segvalues= 0;
    digit2_outmode= 1;
    b1= 0x02; b2= 0x20;
    for (i2= 0; i2< 5; i2++)
    {
      for (i= 0; i< 6; i++)
      {
        digit2_segvalues= (b1 << 8) | b2;
        b1= b1 >> 1;
        if (b1== 0x00) b1= 0x20;
        b2= b2 << 1;
        if (b2== 0x40) b2= 0x01;
        delay(70);
      }
    }
    delay(1000);
  }
}
