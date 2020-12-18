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

     ---------------------------------------------------

     Frequenzzaehlereingang ist PB2
     ( Pin 5 bei DIL14 Gehaeuse)

     ---------------------------------------------------

     Fuer den Voltage-Mode ist am ADC-Eingang (Channel 3
     = PA3) ein Spannungsteiler mit 1M Ohm zu 68k Ohm vor-
     zuschalten

     11.11.2019 R. Seelig
   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "tm16xx.h"
#include "avr_gpio.h"
#include "adc_single.h"

volatile uint32_t  fc;
volatile uint32_t  freq;

#define modesel1_init()       PA7_input_init()
#define is_modesel1()         is_PA7()
#define modesel2_init()       PA0_input_init()
#define is_modesel2()         is_PA0()

#define delay                 _delay_ms


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
#define  u_in                 1710000              // 17.27 entsprechen Digitalwert 1023 (0x400)
#define  u_in_raw              110000              // 1.1V entsprechen Digitalwert 1023

#define  bin2spg(value)       ( value * ( u_in / 1023 ) / 1000 )
#define  bin2spg_raw(value)   ( value * ( u_in_raw / 1023 ) / 1000 )


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
void voltage_mode(uint8_t raw)
{
  uint16_t mwert;

  adc_init(2, 3);               // ADC initialisieren mit 1,1V interner Referenz auf Kanal3 (Pin PA3)

  tm16_xofs= 2;                 // fuer Spannungsmessung werden nur 4 Digits benoetigt, die beiden
                                // linken Digits werden dunkel geschaltet
  while(1)
  {
    delay(400);
    mwert= get_mwert();
    if (!raw)
      tm16_setdez(bin2spg(mwert),3 );
    else
      tm16_setdez(bin2spg_raw(mwert),3 );
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

/* --------------------------------------------------------
         ab hier Funktionen fuer Temperaturmessung
   -------------------------------------------------------- */

/* -------------------------------------------------
     Lookup-table fuer NTC-Widerstand
     R25-Wert: 10.00 kOhm
     Pullup-Widerstand: 10.00 kOhm
     Materialkonstante beta: 3950
     Aufloesung des ADC: 10 Bit
     Einheit eines Tabellenwertes: 0.1 Grad Celcius
     Temperaturfehler der Tabelle: 0.5 Grad Celcius
   -------------------------------------------------*/
const int PROGMEM ntctable[] = {
  1269, 1016, 763, 621, 520, 439, 370, 308,
  250, 194, 139, 83, 22, -47, -132, -256,
  -380
};

/* -------------------------------------------------
                     ntc_gettemp

    zuordnen des Temperaturwertes aus gegebenem
    ADC-Wert.
   ------------------------------------------------- */
int ntc_gettemp(uint16_t adc_value)
{
  int p1,p2;

  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.
  p1 = pgm_read_word(&(ntctable[ (adc_value >> 6)    ]));
  p2 = pgm_read_word(&(ntctable[ (adc_value >> 6) + 1]));

  // zwischen beiden Punkten interpolieren.
  return p1 - ( (p1-p2) * (adc_value & 0x003f) ) / 64;
}
/* --------------------------------------------------------
                           temp_out

     Ausgabe eines Temperaturwertes auf der 7-Segment-
     anzeige (inkl. Einheitenanzeige).

     Uebergabe:
       tempval : anzuzeigender Temperaturwert als Fest-
                 kommazahl. Bsp.: 214 wird als 21,4 oC
                 angezeigt.
   -------------------------------------------------------- */
void temp_out(int16_t tempval)
{
  tm16_xofs= -2;                           // Anzeigeposition einer 4-stelligen Zahl linksbuendig
  tm16_setdez6digit_nonull(tempval, 2);    // Zahlenausgabe mit Kommaanzeige an der 2. Position
  tm16_xofs= 0;
  tm16_setbmp(4, 0x63);                    // Bitmap fuer hochgestelltes o
  tm16_setbmp(5, 0x39);                    // Bitmap fuer "C"
  if (tempval< 0) tm16_setbmp(0,0x40);
}


/* --------------------------------------------------------
                          temp_mode

     Temperatur in einer Endlosschleife messen und anzeigen
   -------------------------------------------------------- */
void temp_mode(void)
{
  int16_t mwert;
  int16_t twert;

  adc_init(0, 3);               // ADC initialisieren mit Vcc als Referenz auf Kanal3 (Pin PA3)

  while(1)
  {
    delay(400);
    mwert= get_mwert();         // ADC-Wert im Mittelwert einlesen
    twert= ntc_gettemp(mwert);  // ADC-Wert nach Temperatur wandeln
    temp_out(twert);            // Temperatur anzeigen
  }

}

/* ----------------------------------------------------------
                            MAIN
   ---------------------------------------------------------- */
void main(void)
{
  uint8_t mode = 0;

  modesel1_init();
  modesel2_init();
  tm16_setbright(7);
  tm16_init();


  // je nach Jumperkonfiguration wird in mode ein Zahlenwert
  // von 0..3 geliefert.
  if (is_modesel1()) mode |= 0x01;
  if (is_modesel2()) mode |= 0x02;

  // Auswahl der verschiedenen Betriebsarten des Panelmeters
  switch (mode)
  {
    case 0 :
    {
      voltage_mode(0);                  // Spannungsmessung mit Spannungsteiler (0..15V)
      break;
    }
    case 1 :
    {
      fcount_mode();
      break;
    }
    case 2 :
    {
      temp_mode();
      break;
    }
    case 3 :
    {
      voltage_mode(1);                  // Spannungsmessung ohne Spannungsteiler (0..1,1V)
      break;
      /*
      // keine vierte Funktion vorhanden
      // Bitmapausgabe Error
      tm16_setbmp(1, 0x79);                    // "E"
      tm16_setbmp(2, 0x50);                    // "r"
      tm16_setbmp(3, 0x50);                    // "r"
      tm16_setbmp(4, 0x5c);                    // "o"
      tm16_setbmp(5, 0x50);                    // "r"
      break;
      while(1);
      */
    }
  }

  while(1);
}
