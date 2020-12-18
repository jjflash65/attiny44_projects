/* -------------------------------------------------------
                           hx1838.c

     Softwaremodul zum HX1838 IR-Receiver

     Belegt zur Auswertung des Receivers Timer1, IR-
     Receiver loest am angeschlossenen Pin einen
     Pinchange Interrupt aus

     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz
     Fuses :  fuer 8 MHz intern
              lo 0xe2
              hi 0xdf

     24.09.2018  R. Seelig
   ------------------------------------------------------ */

#include "hx1838.h"

volatile uint16_t  ir_code;                                                 // Code des letzten eingegangenen 16-Bit Wertes
volatile uint8_t   ir_newflag;                                              // zeigt an, ob ein neuer Wert eingegangen ist

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

     Initialisiert den Timer1 (16-Bit) mit einer Takt-
     rate von 128u us

     Uebergabe:
        prescale  : Teilerfaktor von F_CPU mit der der
                    Timer getaktet wird.
   ------------------------------------------------------- */
void timer1_init(uint8_t prescale)
{
  /*
  TCCR1B : Timer/Counter Controllregister B
    u.a. verantwortlich fuer das Einstellen der Taktversorgung des Zaehlers.
    Bits sind: CS02..CS00
       1 : keine Teilung, 2 : div(8), 3 : div(64), 4 : div(256), 5 : div(1024)

    Bsp.:
       F_CPU    = 8 MHz, prescaler 1024 = 7812,5 Hz = 128uS Taktzeit Zaehler
  */
  TCCR1B = prescale;
  TCNT1H = 0;                  // Zaehler zuruecksetzen
  TCNT1L = 0;
}


/* --------------------------------------------------
                      waittil_hi

     wartet, bis die Signalleitung zu logisch 1
     wird.

     Uebergabe:
        timeout  : Zeit x 100us nach der nach nicht
                   eingehen eines Hi-Pegels ein
                   Timeout ausgeloest wird
   -------------------------------------------------- */
uint8_t waittil_hi(uint16_t timeout)
{
  tim1_clr();
  while ( (!(is_irin() )) && (timeout > tim1_getvalue()) );
  if (timeout > tim1_getvalue()) return 0; else return 1;
}

/* --------------------------------------------------
                      waittil_lo

     wartet, bis die Signalleitung zu logisch 0
     wird.

     Uebergabe:
        timeout  : Zeit x 100us nach der nach nicht
                   eingehen eines Lo-Pegels ein
                   Timeout ausgeloest wird
   -------------------------------------------------- */
uint8_t waittil_lo(uint16_t timeout)
{
  tim1_clr();
  while ( (is_irin() ) && (timeout > tim1_getvalue()) );
  if (timeout > tim1_getvalue()) return 0; else return 1;
}

/* --------------------------------------------------
                      ir_getbit

     liefert je nach Pulselaenge des IR-Receivers 0
     (kurzer Impuls) oder 1 (langer Impuls). Bei
     Timeoutueberschreitung wird 2 zurueck geliefert

     Uebergabe:
        timeout  : Zeit x 100us nach der nach nicht
                   eingehen eines Lo-Pegels ein
                   Timeout ausgeloest wird
     Rueckgabe   : 0,1  Datenbit
                     2  Timeout aufgetreten
   -------------------------------------------------- */
uint8_t ir_getbit(uint16_t timeout)
{
  volatile uint16_t t;

  tim1_clr();
  while ( (is_irin() ) && (timeout > tim1_getvalue()) );
  t= tim1_getvalue();
  if (timeout <= t) return 2;
  if (t< 9) return 0; else return 1;
}

/* --------------------------------------------------
                    pinchange_init
     festlegen, dass Datenanschluss des IR-Receivers
     einen Pinchange Interrupt ausloest
   -------------------------------------------------- */
void pinchange_init(void)
{
  IR_PCMSK |= (1 << IR_PCINT);
  GIMSK |= (1 << IR_PCIE);

  ir_input_init();
}

/* --------------------------------------------------
                    pinchange_deinit
     Pinchange-Interrupt deaktivieren
  -------------------------------------------------- */
void pinchange_deinit(void)
{
  IR_PCMSK &= ~(1 << IR_PCINT);
  GIMSK &= ~(1 << IR_PCIE);

  ir_input_init();
}


/* --------------------------------------------------
                  IR_ISR_vect

     ISR fuer Pinchangevektor Datapin des
     IR-Receivers

     Dauer Lo-Pegel vor Startbit:   9 ms
     Startbit (Hi)              : 4.5 ms
     Datenbit Lo                : 0.6 ms
     Datenbit Hi                : 1.7 ms

     In der Interruptroutine werden die globalen
     Variablen ir_code und ir_newflag geschrieben,
     die in einem Hauptprogramm gepollt werden
     koennen.
   -------------------------------------------------- */
ISR (IR_ISR_vect)
{
  volatile uint8_t cx, b, hw;
  volatile uint16_t result;

  if (!(is_irin()) )                                   // ist der Datenpin des IR-Receivers zu 0 geworden
  {
    pinchange_deinit();                                // Interruptfaehigkeit des Pins aus

    if ( waittil_hi(93) ) goto timeout_err;            // auf Startbit des Frames warten (nach 93*.128us = 12 ms Timeout)
    if ( waittil_lo(47) ) goto timeout_err;            // auf Ende Startbit des Frames warten (nach 6 ms Timeout)
    if ( waittil_hi(24) ) goto timeout_err;            // auf erstes Datenbit warten (nach 3 ms Timeout)

    // die ersten 16 Bit des Pakets einlesen ohne diese
    // auszuwerten
    for (cx= 0; cx < 16; cx++)
    {
      b= ir_getbit(24);
      if (b == 127) goto timeout_err;
      if ( waittil_hi(24) ) goto timeout_err;          // auf naechstes Datenbit warten
    }

    // erste 8 Bit seriell lesen
    hw= 0;
    for (cx= 0; cx < 8; cx++)
    {
      b= ir_getbit(24);
      if (b == 127) goto timeout_err;
      hw |= (b << cx);
      if ( waittil_hi(24) ) goto timeout_err;          // auf naechstes Datenbit warten
    }
    result= (uint16_t) hw << 8;

    // zweite 8 Bit seriell lesen
    hw= 0;
    for (cx= 0; cx < 8; cx++)
    {
      b= ir_getbit(24);
      if (b == 127) goto timeout_err;
      hw |= (b << cx);
      if ( waittil_hi(24) ) goto timeout_err;          // auf naechstes Datenbit warten
    }
    result |= hw;

    if (result < 0x8000)
    {
      ir_code= result;
      ir_newflag= 1;
    }

    timeout_err:

//      delay(20);
      pinchange_init();
//      delay(20);
  }
}

/* -------------------------------------------------
                       hex1838_init
     festlegen, dass Datenanschluss des IR-Receivers
     einen Pinchange Interrupt ausloest
   -------------------------------------------------- */
void hx1838_init(void)
{
  timer1_init(pscale1024);
  pinchange_init();
}
