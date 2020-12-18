/* ----------------------------------------------------------
                             analog.c

     Versuche zur Ausgabe analoger Spannungen am
     ATtiny44 mittels PWM

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf

     14.09.2018    R. Seelig
   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <math.h>

#include "avr_gpio.h"

#define pwmdir      DDRA
#define pwmport     PORTA
#define pwmout      PA6                       //  PA6 fuer OC1A (PWM-Out)


#define u_hipegel   4.77

/* -----------------------------------------------------
                       pwm_init

    initialisiert einen PWM Generator mit angegebenem
    PWM Ausgangspin (bei ATtiny44 ist das PA6

    Uebergabe:
       Der Grundtakt des internen PWM Zaehlers ist
       vom CPU-Takt abhaengig. Dieser Takt kann
       geteilt zugefuehrt werden

       prescale:
         1 : keine Taktteilung     2 : F_CPU / 8
         3 : F_CPU / 64            4 : F_CPU / 256
         5 : F_CPU / 1024
   ----------------------------------------------------- */
void pwm_init(uint8_t prescale)
{
   pwmdir |= (1 << pwmout);                   // OC1A .. PA6 als PWM-Ausgang

   // WGM10= 0 ; WGM11 WGM12, WGM13 = 1 ===> Modus 14 (Fast PWM)
   // Prescaler wird durch CS10, CS11, CS12 gesetzt

   TCCR1A = (1<<COM1A1) | (1<<WGM11);
   TCCR1B = (1<<WGM12) | (1<<WGM13) | prescale;
}

/* -----------------------------------------------------
                          setpwm

     setzt die Parameter fuer Gesamtlaenge und Pulsdauer
     einer Pulsweitenmodulation.

     Uebergabe:
       tim1 : Gesamtdauer (in Digits)
       tim2 : Pulsdauer (in Digits)

       tim1 / tim2 ist somit das Verhaeltnis von Ge-
       samtdauer zu Pulsdauer. Wie lange die Gesamt-
       dauer ist (65536 Digits) ist von pwm_init ab-
       haengig.
   ----------------------------------------------------- */
void setpwm(uint16_t tim1, uint16_t tim2)
{
  // tim1 ==> Gesamtdauer
  // tim2 ==> Pulsdauer

  ICR1H = (tim1 >> 8);
  ICR1L = (uint8_t) tim1 & 0x00ff;
  OCR1AH = (tim2 >> 8);
  OCR1AL = (uint8_t) tim2 & 0x00ff;
}


/* -----------------------------------------------------
                        analogout

     wird an den PWM-Pin ein passiver Tiefpass ange-
     schlossen, so kann hier am Kondensator die analoge
     Ausgangsspannung abgegriffen werden. Somit fungiert
     der PWM-Ausgang als digital-analog Wandler

     Uebergabe:
       value   : der Spannungswert, der Ausgegeben werden
                 soll.
       refwert : die Spannung, die ein logischer Hi-Pegel
                 besitzt
   ----------------------------------------------------- */
void analogout(float value, float refwert)
{
  uint16_t regwert;
  uint16_t regout;
  float    r;

  r= (value * 1023) / refwert;
  regout= r;

  setpwm(1023,regout);

}


/* ---------------------------------------------------------------------------
                                    M A I N

     an PA6 kann (nach einem Tiefpass) fuer 4 Sekunden der Spannungswert
     1,75V gemesen werden. Danach wird in 10 Schritten die Ausgangsspannung
     von 0V nach 5V erhoeht. Anschliessend wird die Ausgangsspannung auf
     2V eingestellt.

     Wird anstelle eines Voltmeters eine LED angeschlossen, aendert sich
     die Helligkeit entsprechend
   --------------------------------------------------------------------------- */
int main(void)
{
  uint16_t  b;
  float     uout;

  pwm_init(1);

  analogout(1.75, u_hipegel);

  _delay_ms(4000);

  uout= 0.0;
  for (b= 0; b< 10; b++)
  {

    analogout(uout, u_hipegel);

    uout= uout + 0.5;
    _delay_ms(1400);
  }
  analogout(2.0, u_hipegel);

  while(1)
  {
    // Wert ansteigen lassen
    for (b= 0; b< 1024; b++)
    {
      setpwm(1023, b);
      _delay_ms(2);
    }
    for (b= 1023; b> 1; b--)
    {
      setpwm(1023, b);
      setpwm(1023, b);
      _delay_ms(2);
    }
  }
}
