/* ----------------------------------------------------------
                      tm16_fcount

     Einfacher Frequenzzaehler mit LED 7-Segmentanzeige
     ueber TM1637 Anzeigentreiber

     MCU   : ATtiny44
     F_CPU : 8MHz intern oder 16 MHz extern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz 16 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung

     ATtiny44         TM16xx
     ---------------------------
        PA1 (12)       CLK
        PA2 (11)       DIO

     PA1 und PA2 sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen

     11.11.2019 R. Seelig
   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "tm16xx.h"
#include "avr_gpio.h"

volatile uint32_t  fc;
volatile uint32_t  freq;

#define modesel_init()       PA7_input_init()
#define is_voltage_mode()    is_PA7()

#define delay                _delay_ms


/* -------------------------------------------------------
                      u_in und Makro bin2spg

    u_in:
    Spannung in 0,1 mV, repraesentiert den Spannungswert
    der dem 10-Bit Maximalwert (1023) entspricht.

    bin2spg:
    rechnet den Wert von u_in bei einer Aufloesung von
    10 Bit in einen Festkommazahlen - Spannungswert um.
    Wert von bspw. 382 entspricht hier dann 3.82 Volt

    Hinweis: Nach dieser Methode koennen bei Messbereich
    5V max. 2% Fehler durch Rundung entstehen
   ------------------------------------------------------- */
#define  u_in                 1666000              // 17.27 entsprechen Digitalwert 1023 (0x400)
#define  bin2spg(value)       ( value * ( u_in / 1023 ) / 1000 )


/* --------------------------------------------------------
                     Interruptvector Timer1

     uebernimmt die Frequenzzaehlervariable in die Variable
     die angezeigt wird und setzt die Zaehlervariable
     zurueck
   -------------------------------------------------------- */
ISR (TIM1_COMPA_vect)
{
  freq= fc;
  fc= 0;
}

/* --------------------------------------------------------
                         timer1_init

     Initialisierung des Timer1 fuer Interruptaufruf
     jede Sekunde (als Compare-A)
   -------------------------------------------------------- */
void timer1_init(void)
{
  /* ---------------------------------------------
     CS10 / CS11 / CS12 sind Steuerbits
     des Taktvorteilers.

     CS 12 | CS11 | CS10
     ----------------------------------------------
       0      0      0   kein Takt, Timer gestoppt
       0      0      1   clk / 1 (kein Teiler)
       0      1      0   clk / 8
       0      1      1   clk / 64
       1      0      0   clk / 256
       1      0      1   clk / 1024
     ---------------------------------------------- */
  TCCR1B = 1 << WGM12 | 1 << CS10 | 1 << CS12;       // Taktvorteiler = 1024
  OCR1A = F_CPU / 1024;
  TCNT1 = 0;

  TIMSK1 = 1 << OCIE1A;
  sei();
}

/* --------------------------------------------------------
                           int0_init

     initialisiert externer Interrrupt 0 fuer Interrupt-
     aufruf bei negativer Flanke

     INT0 - Pin ist PB2 ( Pin 5 bei DIL14 Gehaeuse)
   -------------------------------------------------------- */
void int0_init(void)
{

//  Interruptaufruf bei Aenderung des Logiklevels
//  MCUCR |= (1 << ISC00);
//  MCUCR &= ~(1 << ISC01);

//  Interruptaufruf bei negativer Flanke
  MCUCR |= (1 << ISC01);
  MCUCR &= ~(1 << ISC00);

  GIMSK |= (1 << INT0);
  sei();
}

/* --------------------------------------------------------
                   Interruptvector ex. INT0

     zaehlt die Frequenzzaehlervariable hoch
   -------------------------------------------------------- */

ISR (INT0_vect)
{
  fc++;
}

/* --------------------------------------------------------
                          get_mwert

     bildet den Mittelwert aus 10 gelesenen ADC-Werten
     und gibt diesen als Argument zurueck
   -------------------------------------------------------- */
uint16_t get_mwert(void)
{
  uint8_t i;
  uint16_t mittelw;

  mittelw= 0;
  for (i= 0; i< 10; i++)
  {
    mittelw += adc_getvalue();
    delay(10);
  }
  return mittelw / 10;
}

/* --------------------------------------------------------
                           voltage_mode

     das Modul funktioniert als Voltage-Meter und zeigt
     in der Schleife die gemessene Spannung an
   -------------------------------------------------------- */
void voltage_mode(void)
{
  uint16_t mwert;

  adc_init(2, 3);               // ADC initialisieren mit 1,1V interner Referenz auf Kanal3 (Pin PA3)

  tm16_xofs= 2;                 // fuer Spannungsmessung werden nur 4 Digits benoetigt, die beiden
                                // linken Digits werden dunkel geschaltet
  while(1)
  {
    delay(400);
    mwert= get_mwert();
    tm16_setdez(bin2spg(mwert),3 );
  }
}

/* --------------------------------------------------------
                           fcount_mode

     das Modul funktioniert als Frequenzzaehler und zeigt
     in der Schleife die gemessene Frequenz an.
   -------------------------------------------------------- */
void fcount_mode(void)
{
  timer1_init();
  int0_init();


  while(1)
  {
    tm16_setdez6digit_nonull(freq,0);
    delay(490);
  }
}

/* ----------------------------------------------------------
                            MAIN
   ---------------------------------------------------------- */
void main(void)
{

  modesel_init();
  tm16_setbright(7);
  tm16_init();

  if (is_voltage_mode()) voltage_mode();
                    else fcount_mode();

  while(1);
}
