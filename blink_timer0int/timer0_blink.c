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
#include <avr/interrupt.h>

#include "avr_gpio.h"


#define led_init()     PA4_output_init()
#define led_clr()      PA4_set()
#define led_set()      PA4_clr()

uint8_t toggleflag = 0;

enum { pscale1 = 1, pscale8, pscale64, pscale256, pscale1024 };

/* -------------------------------------------------------
         Interruptvektor, Timer0 compare match

         dieser Vektor wird alle 2,048 mS aufgerufen
   ------------------------------------------------------- */
ISR (TIM0_COMPA_vect)
{
  volatile static uint16_t intcx = 0;

  intcx++;
  if (intcx == 250)
  {
    intcx= 0;
    if (toggleflag & 1)
    {
      led_set();
      toggleflag= 0;
    }
    else
    {
      led_clr();
      toggleflag= 1;
    }
  }
  TCNT0= 0;
}

/* -------------------------------------------------------
                        timer0_init

     Initialisiert den Timer0 (8-Bit) als Comparetimer.

     Uebergabe:
        prescale  : Teilerfaktor von F_CPU mit der der
                    Timer getaktet wird.
        compvalue : Vergleichswert, bei dem ein Interrupt
                    ausgeloest wird
   ------------------------------------------------------- */
void timer0_init(uint8_t prescale, uint8_t compvalue)
{
  /*
  TCCR0B : Timer/Counter Controllregister B
    u.a. verantwortlich fuer das Einstellen der Taktversorgung des Zaehlers.
    Bits sind: CS02..CS00
       1 : keine Teilung, 2 : div(8), 3 : div(64), 4 : div(256), 5 : div(1024)
  OCR0A : Output Compare Register, eines von zwei Vergleichsregistern
    erreicht der Zaehler TCNT0 den Wert von OCR0A kann ein Interrupt ausgeloest
    werden (wenn dieses im TIMSK0 Register aktiviert ist)
  TIMSK0 : Timer Interrupt Mask Register
    bestimmt ob und bei welchem Ereignis ein Interrupt ausgeloest werden soll.
    OCIE0A (Bit1) loest bei Gleichheit von OCR0A und TCNT0 einen Interrupt aus.
    Interruptvektor fuer OCIE0A ist:   ISR (TIMER0_COMPA_vect)   (fuer ATtiny44)

    Bsp.:
       F_CPU = 8 MHz, prescaler 1024 = 7812,5 Hz = 128uS Taktzeit Zaehler
       OCR0A = 16: nach 128uS * 16 = 2,048 mS wird ein Interupt ausgeloest (bei
               Startwert TCNT0 = 0).
  */
  TCCR0B = prescale;
  OCR0A = compvalue;
  TCNT0 = 0;                   // Zaehler zuruecksetzen

  TIMSK0 = 1 << OCIE0A;        // if OCR0A == TCNT0 dann Interrupt
  sei();                       // Interrupts grundsaetzlich an

}

/* ----------------------------------------------------------------------------------
                                        MAIN
   ---------------------------------------------------------------------------------- */
int main(void)
{

  led_init();
  timer0_init(pscale1024, 16);     // F_CPU / 1024 => .128 mS * 16 = Interruptintervall

  while(1);                        // Endlosschleife, blinken uebernimmt der Interrupt
}
