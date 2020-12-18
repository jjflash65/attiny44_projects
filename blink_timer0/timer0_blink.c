/* ----------------------------------------------------------

   Das "Hallo Welt" der Microcontroller: Blinkende LED als
   Beispiel fuer den Einsatz des Timer0 im Compare-Match
   Interruptbetrieb

   Hardware :  - LED

   MCU      :  Attiny44
   Takt     :  8 MHz intern

   Fuses    :  Lo:0xE2    Hi:0xDF

   14.08.2018 R. Seelig
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
*/


#include <avr/io.h>

#include "avr_gpio.h"


#define led_init()     PA4_output_init()
#define led_clr()      PA4_set()
#define led_set()      PA4_clr()

uint8_t toggleflag = 0;

enum { pscale1 = 1, pscale8, pscale64, pscale256, pscale1024 };


/* -------------------------------------------------------
                        timer0_init

     Initialisiert den Timer0 (8-Bit) als Comparetimer.

     Uebergabe:
        prescale  : Teilerfaktor von F_CPU mit der der
                    Timer getaktet wird.
        compvalue : Vergleichswert, bei dem ein Interrupt
                    ausgeloest wird
   ------------------------------------------------------- */
void timer0_init(uint8_t prescale)
{
  /*
  TCCR0B : Timer/Counter Controllregister B
    u.a. verantwortlich fuer das Einstellen der Taktversorgung des Zaehlers.
    Bits sind: CS02..CS00
       1 : keine Teilung, 2 : div(8), 3 : div(64), 4 : div(256), 5 : div(1024)

    Bsp.:
       F_CPU = 8 MHz, prescaler 1024 = 7812,5 Hz = 128uS Taktzeit Zaehler
  */
  TCCR0B = prescale;
  TCNT0 = 0;                   // Zaehler zuruecksetzen
}


/* -------------------------------------------------------
                        timer0_wait1ms

     Timer0 ist mit 0.125 uS getaktet. Eine Zaehlung bis
     800 ergibt 1 ms
   ------------------------------------------------------- */
void timer0_wait1ms(void)
{
  uint16_t cx;

  for (cx= 0; cx < 51; cx++)     // 50 * 20 us = 1 ms
  {
    TCNT0= 10;                   // Zaehler vorbelegen
    while (TCNT0 != 170);        // und 160 * 0.125 us = 20 us warten
  }
}

/* ----------------------------------------------------------------------------------
                                        MAIN
   ---------------------------------------------------------------------------------- */
int main(void)
{
  uint16_t cx;
  uint8_t  ledflag;

  led_init();
  timer0_init(pscale1);                  // F_CPU= 8MHz / 1 => 0.125 uS

  ledflag= 0;
  while(1)
  {
    for (cx= 0; cx< 500; cx++)
      timer0_wait1ms();
    if (ledflag) { ledflag= 0; led_set(); }
            else { ledflag= 1; led_clr(); }
  }
}
