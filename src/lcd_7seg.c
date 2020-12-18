/* -------------------------------------------------------
                        lcd_7segdemo.c

     Softwaremodul zur Anbindung eines zweistelligen
     7-Segment LCD's OHNE eigenen Controller ("pures Display")
     ueber 2 Schieberegister SN74HC595

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegung siehe lcd_7segdemo.h

     05.11.2018  R. Seelig
   ------------------------------------------------------ */

#include "lcd_7seg.h"


// Bitmapmuster der Ziffern
uint8_t  lcd7s_bmp[16] =
            { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
              0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

uint8_t lcd7s_buffer = 0x00;
uint8_t lcd7s_dp = 0;


/* ----------------------------------------------------------
   lcd7s_ckpuls

   nach der Initialisierung besitzt die Taktleitung low-
   Signal. Hier wird ein Taktimpuls nach high und wieder
   low erzeugt
   ---------------------------------------------------------- */
void lcd7s_ckpuls(void)
// Schieberegister Taktimpuls
{
  srclock_set();
  srclock_clr();
}

/* ----------------------------------------------------------
   lcd7s_stpuls

   nach der Initialisierung besitzt die Strobeleitung low-
   Signal. Hier wird ein Taktimpuls nach high und wieder
   low erzeugt
   ---------------------------------------------------------- */
void lcd7s_stpuls(void)
// Strobe Taktimpuls
{
  srstrobe_set();
  srstrobe_clr();
}

/* ----------------------------------------------------------
   lcd7s_outbyte

   uebertraegt das Byte in - value - in das Schieberegister.
   ---------------------------------------------------------- */
void lcd7s_outbyte(uint8_t hi_value, uint8_t lo_value)
{
  uint16_t mask, value;
  uint8_t  b;

  mask= 0x8000;
  value= hi_value;
  value = (value << 8) | lo_value;

  for (b= 0; b< 16; b++)
  {
    // Byte ins Schieberegister schieben, MSBit zuerst
    if (mask & value) srdata_set();           // 1 oder 0 entsprechend Wert setzen
                 else srdata_clr();

    lcd7s_ckpuls();                             // ... Puls erzeugen und so ins SR schieben
    mask= mask >> 1;                             // naechstes Bit
  }
}

/* ----------------------------------------------------------
   lcd7s_dezout

   gibt 2-stelligen dezimalen Wert auf Segmentanzeige aus
   ---------------------------------------------------------- */
void lcd7s_dezout(uint8_t value)
{
  lcd7s_buffer= value % 10;
  lcd7s_buffer |= ((value /10) << 4);
}

/* ----------------------------------------------------------
   lcd7s_hexout

   gibt 2-stelligen hexadezimalen Wert auf Segmentanzeige aus
   ---------------------------------------------------------- */
void lcd7s_hexout(uint8_t value)
{
  lcd7s_buffer= value;
}


/* -------------------------------------------------------
         Interruptvektor, Timer0 compare match

         Intervall wird durch timer0_init bestimmt (hier
         etwa alle 2,048 ms)
   ------------------------------------------------------- */
ISR (TIM0_COMPA_vect)
{
  volatile static uint8_t toggleflag= 0;
  volatile uint8_t hi, lo;

  lo= lcd7s_bmp[lcd7s_buffer & 0x0f];
  hi = lcd7s_bmp[lcd7s_buffer >> 4];
  if (lcd7s_dp) lo = lo | 0x80;

  if (toggleflag)
  {
    toggleflag--;
    lcd7s_outbyte(hi & 0x7f, lo);               // & 0x7f => BP = 0
    lcd7s_stpuls();
  }
  else
  {
    lcd7s_outbyte(~(hi) | 0x80, ~(lo));         // | 0x80 => BP = 1
    lcd7s_stpuls();
    toggleflag++;
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

/* ----------------------------------------------------------
                          lcd7s_init

     initialisiert die Pins des Controllers, an den das
     kaskadierte Schieberegister angeschlossen ist als
     Ausgaenge und startet den Timer0 Interrupt
   ---------------------------------------------------------- */
void lcd7s_init(void)
{
  sr_init();
  timer0_init(4,64);
}
