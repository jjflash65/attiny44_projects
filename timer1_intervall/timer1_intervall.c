/* ----------------------------------------------------------

   Zaehlt 16 Bit Variable im 100 uS Intervall hoch

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

volatile static uint16_t tim1_intervall;


enum { pscale1 = 1, pscale8, pscale64, pscale256, pscale1024 };


/* -------------------------------------------------------
         Interruptvektor, Timer1 compare match

     dieser Vektor wird alle mS aufgerufen, intcnt
     jede ms inkrementiert
   ------------------------------------------------------- */
ISR (TIM1_COMPA_vect)
{
  tim1_intervall++;
  TCNT1H= 0;
  TCNT1L= 0;
}

/* -------------------------------------------------------
                        timer1_init

     Initialisiert den Timer1 (16-Bit) als Comparetimer.

     Uebergabe:
        prescale  : Teilerfaktor von F_CPU mit der der
                    Timer getaktet wird.
        compvalue : Vergleichswert, bei dem ein Interrupt
                    ausgeloest wird
   ------------------------------------------------------- */
void timer1_init(uint8_t prescale, uint16_t compvalue)
{
  /*
  TCCR1B : Timer/Counter Controllregister B
    u.a. verantwortlich fuer das Einstellen der Taktversorgung des Zaehlers.
    Bits sind: CS02..CS00
       1 : keine Teilung, 2 : div(8), 3 : div(64), 4 : div(256), 5 : div(1024)

  OCR1AH:CR1AL : Output Compare Register AH:AL bilden zusammen einen 16-Bit
    Vergleichswert. Eerreicht der Zaehler TCNT1H:TCNT1L den Wert von OCR1AL:OCR1AH
    wird ein Interrupt ausgeloest (wenn dieses im TIMSK1 Register aktiviert ist)

  TIMSK1 : Timer Interrupt Mask Register
    bestimmt ob und bei welchem Ereignis ein Interrupt ausgeloest werden soll.
    OCIE0A (Bit1) loest bei Gleichheit von OCR1AH:L und TCNT1H:L einen Interrupt aus.
    Interruptvektor fuer OCIE1A ist:   ISR (TIMER1_COMPA_vect)   (fuer ATtiny44)

    Bsp.:
       F_CPU    = 8 MHz, prescaler 1024 = 7812,5 Hz = 128uS Taktzeit Zaehler
       OCR1AH:L = 16: nach 128uS * 16 = 2,048 mS wird ein Interupt ausgeloest (bei
                  Startwert TCNT1H:L = 0).
  */
  TCCR1B = prescale;
  OCR1AH = (compvalue >> 8);
  OCR1AL = (compvalue & 0x00ff);
  TCNT1H = 0;                  // Zaehler zuruecksetzen
  TCNT1L = 0;

  TIMSK1 = 1 << OCIE1A;        // if OCR0AH:L == TCNT1H:L dann Interrupt
  sei();                       // Interrupts grundsaetzlich an

}

/* ----------------------------------------------------------------------------------
                                        MAIN
   ---------------------------------------------------------------------------------- */
int main(void)
{

  led_init();
  timer1_init(pscale1, 8000);       // F_CPU / 1  => .125 us * 8000 = 1 ms Interruptintervall

  led_clr();

  while(1)
  {
    tim1_intervall= 0;
    while(tim1_intervall != 500);
    led_set();
    tim1_intervall= 0;
    while(tim1_intervall != 500);
    led_clr();
  }
}

