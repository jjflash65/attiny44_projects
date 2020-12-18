/* ----------------------------------------------------------
                        seg7_2digit595.c


     Softwaremodul zur Ansteuerung einer 2-stelligen
     7-Segmentanzeige mit gemeinsamer Kathode ueber ein
     einzelnes SN74HC595  Schieberegister

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     24.10.2018  R. Seelig


     Anschluss / Pinbelegung siehe seg7_digit595.h
   ---------------------------------------------------------- */

#include "seg7_2digit595.h"


// Bitmapmuster der Ziffern
uint8_t  led7sbmp[16] =
            { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
              0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

uint8_t    digit2_outmode = 0;                // 0 : Ziffernausgabe
                                              // 1 : Ansteuerung einzelner Segmente (mit Bitmuster in
                                              //     digit2_segvalues)

uint8_t    digit2_value;                      // beinhaltet bei Ziffernausgabe den auszugebenden Zahlenwert
uint16_t   digit2_segvalues = 0x0000;         // beinhaltet bei Segmentausgabe das auszugebende Bitmuster



/* ----------------------------------------------------------
   digit2_ckpuls

   nach der Initialisierung besitzt die Taktleitung low-
   Signal. Hier wird ein Taktimpuls nach high und wieder
   low erzeugt
   ---------------------------------------------------------- */
void digit2_ckpuls(void)
// Schieberegister Taktimpuls
{
  srclock_set();
  srclock_clr();
}

/* ----------------------------------------------------------
   digit2_stpuls

   nach der Initialisierung besitzt die Strobeleitung low-
   Signal. Hier wird ein Taktimpuls nach high und wieder
   low erzeugt
   ---------------------------------------------------------- */
void digit2_stpuls(void)
// Strobe Taktimpuls
{
  srstrobe_set();
  srstrobe_clr();
}

/* ----------------------------------------------------------
   digit2_outbyte

   uebertraegt das Byte in - value - in das Schieberegister.
   ---------------------------------------------------------- */
void digit2_outbyte(uint8_t value)
{
  uint8_t mask, b;

  mask= 0x80;

  for (b= 0; b< 8; b++)
  {
    // Byte ins Schieberegister schieben, MSB zuerst
    if (mask & value) srdata_set();              // 1 oder 0 entsprechend Wert setzen
                 else srdata_clr();

    digit2_ckpuls();                             // ... Puls erzeugen und so ins SR schieben
    mask= mask >> 1;                             // naechstes Bit
  }
}

/* ----------------------------------------------------------
   digit2_init

   initalisiert alle GPIO Pins die das Schieberegister
   benoetigt als Ausgaenge und setzt alle Ausgaenge des
   Schieberegisters auf 0
   ---------------------------------------------------------- */
void digit2_init(void)
// alle Pins an denen das Modul angeschlossen ist als
// Ausgang schalten
{
  srdata_init();
  srstrobe_init();
  srclock_init();

  srdata_clr();
  srclock_clr();
  srstrobe_clr();

  digit2_outbyte(0);
}

/* ----------------------------------------------------------
   digit2_dezout

   gibt 2-stelligen dezimalen Wert auf Segmentanzeige aus
   ---------------------------------------------------------- */
void digit2_dezout(uint8_t value)
{
  digit2_value= value % 10;
  digit2_value |= ((value /10) << 4);
}

/* ----------------------------------------------------------
   digit2_hexout

   gibt 2-stelligen hexadezimalen Wert auf Segmentanzeige aus
   ---------------------------------------------------------- */
void digit2_hexout(uint8_t value)
{
  digit2_value= value;
}


/* ----------------------------------------------------------
                          timer1_init

     initialisiert den Timer1 Interruptintervall fuer 1mS
   ---------------------------------------------------------- */
void timer1_init(void)
{
  TCCR1B = 1 << WGM12 | 1 << CS10;            // CS10 = F_CPU / 1 => Zaehlertaktfrequenz = F_CPU
  OCR1A = F_CPU / 1000;                       // F_CPU (8MHz) / 1000 = 8000. (1 / 8.000.000 ) * 8000 = 1/1000 Sekunde
  TCNT1 = 0;

  TIMSK1 = 1 << OCIE1A;
  sei();
}


/* ------------------------------------------------------
                       I S R - Timer 1

     gibt gemultiplexed die 2-stellige Zahl in
     digit2_value auf der Anzeige aus
   ------------------------------------------------------ */
ISR (TIM1_COMPA_vect)
{
  static uint8_t segmpx= 0;
  static uint8_t nfcmp = 0;
  static uint8_t lsflag = 0;


  if (!(segmpx))                                               // MSD von globaler Variable digit2_value ausschieben
  {
    if (!(digit2_outmode))
      digit2_outbyte(led7sbmp[ digit2_value >> 4 ]);           // MPX = 0
    else
      digit2_outbyte(digit2_segvalues >> 8);
  }

  else                                                         // LSD ausgeben
  {
    if (!(digit2_outmode))
      digit2_outbyte(led7sbmp[ digit2_value & 0x0f ] | 0x80);  // 0x80 = MPX => 1
    else
      digit2_outbyte((digit2_segvalues & 0x00ff) | 0x80 );
  }
  segmpx++;
  segmpx= segmpx % 2;
  digit2_stpuls();                        // Inhalt Schieberegister ins Latch (und damit anzeigen)

  TCNT1= 0;
}
