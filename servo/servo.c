/* ----------------------------------------------------------
                            servo.c

     Ansteuerung eines Servomotors

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     zusaetzliche Hardware:
                 - Servomotor
                 - Trimmer als Spannungsteiler am
                   Analogeingang PA3

     05.11.2018  R. Seelig

   ---------------------------------------------------------- */

/*
  Ein Servomotor besitzt i.a.R. 3 Anschluesse: +Vcc, GND, Steuersignal.

  Prinzipiell ist das Steuersignal an sich ein PWM-Signal wobei beim
  Servomotor hierbei der Puls zeitkritisch und die Pause hierbei weniger
  zeitkritisch ist. 1ms Puls entspricht hier dem rechten, 2mS Puls
  den linken Anschlag.

  Die Funktion servo_delay benoetigt ca. 2.15 us sodass ein Wert fuer
  servo_value= 200 dem rechten, ein Wert von 800 dem linken Anschlag
  entspricht !
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "adc_single.h"

#define delay           _delay_ms

#define servo_init()    PB0_output_init()
#define servo_pinset()  PB0_set()
#define servo_pinclr()  PB0_clr()

uint16_t servo_value  = 500;


void servo_delay(uint16_t value)
{
  volatile uint16_t i;

  for (i= 0; i< value; i++) _delay_us(1);
}

/* -------------------------------------------------------
         Interruptvektor, Timer0 compare match

         Intervall wird durch timer0_init bestimmt (hier
         dann ca. alle 19mS)

         Hinweis: normalerweise hat eine Delay-Funktion
         in einer Interruptroutine absolut nichts zu
         suchen. Hier wurde zugunsten der einfachen
         Handhabbarkeit eine Ausnahme gemacht von daher
         beachten: diese ISR kann bis zur Beendigung
         2,5 mS benoetigen !
   ------------------------------------------------------- */
ISR (TIM0_COMPA_vect)
{
  servo_pinset();
  servo_delay(servo_value);
  servo_pinclr();

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
       OCR0A = 148: nach 128uS * 148 = 18,944 mS wird ein Interupt ausgeloest (bei
               Startwert TCNT0 = 0).
  */
  TCCR0B = prescale;
  OCR0A = compvalue;
  TCNT0 = 0;                   // Zaehler zuruecksetzen

  TIMSK0 = 1 << OCIE0A;        // if OCR0A == TCNT0 dann Interrupt
  sei();                       // Interrupts grundsaetzlich an

}


/* ----------------------------------------------------------
                             M-A-I-N
   ---------------------------------------------------------- */
int main(void)
{
  uint16_t i;
  uint16_t w;


  servo_init();
  timer0_init(5, 148);
  adc_init(0, 3);                          // Analogeingang auf PA3

  while(1)
  {
    w= ( adc_getvalue() * 17 ) / 29;       // skaliert die Aufloesung des ADC von 0..1023 nach 0..600
                                           // (1024*17) / 29 = 600

    servo_value= w+200;                    // Interrupt stellt Servomotor ein.
    delay(10);
  }

}



