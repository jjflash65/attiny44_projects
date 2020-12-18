/* ----------------------------------------------------------
                           mini_io.c


     Softwaremodul fuer ein kombiniertes

                     "Minimum I/O System"

     Beinhaltet Software fuer eine kombinierte gemultiplexte
     zweistellige LED 7-Segmentanzeige mit maximal zu-
     saetzlichen 8 einzelnen LED's.
     Die Daten fuer die LED-Anzeigen werden ueber ein
     SN74HC595 Schieberegister ausgegeben.
     Alle LED's (auch die der einzelnen) sind als gemein-
     same Kathode implementiert.

     Desweiteren existiert ein Lautsprecheranschluss mittels
     diesem  einfache monophonic Toene erzeugt werden koennen.

     Zusaetzlich in diesem Softwaremodul existiert die
     Abfragemoeglichkeit von 4 User-Tastern.

     Anmerkung: Dadurch, dass mit Timer0 die Frequenzer-
     zeugung des Tons gehandhabt wird und in derselben
     ISR das Multiplexing der Anzeige stattfindet, ist
     die Multiplexfrequenz der Anzeige abhaengig vom ge-
     rade eingestellten Ton.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegungen siehe mini_io.h

     30.10.2018  R. Seelig

   ---------------------------------------------------------- */


#include "mini_io.h"

// ---------------- Variable Anzeige -----------------------

// Bitmapmuster der Ziffern
uint8_t  led7sbmp[16] =
            { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
              0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

uint8_t    digit2_outmode = 0;                // 0 : Ziffernausgabe
                                              // 1 : Ansteuerung einzelner Segmente (mit Bitmuster in
                                              //     digit2_segvalues)

uint8_t    digit2_value;                      // beinhaltet bei Ziffernausgabe den auszugebenden Zahlenwert
uint16_t   digit2_segvalues = 0x0000;         // beinhaltet bei Segmentausgabe das auszugebende Bitmuster
uint8_t    led_anzbuf = 0;

//------------ Variable fuer Tonerzeugung-------------------

uint8_t togglespk;
volatile uint8_t sound;
volatile uint8_t cnt;
volatile uint8_t srout;

int freqreload [24] = { 523,  555,  587,  623,  659,  699,  740,  784,  831,  881,  933, 988,
                        1047, 1109, 1175, 1245, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1975 };


/* --------------------------------------------------------------------
       Funktionen fuer Anzeige
   -------------------------------------------------------------------- */

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

  mpx0_init();
  mpx1_init();
  mpx2_init();

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

/* --------------------------------------------------------------------
       Ende Anzeigefunktionen
   -------------------------------------------------------------------- */

/* ----------------------------------------------------------
   button_get

   liest die Tasten ein und gibt deren Wert im Binaercode
   zurueck
   ---------------------------------------------------------- */
uint8_t button_get(void)
{
  uint8_t code;

  code= 0;
  if is_but0() code = 0x01;
  if is_but1() code |= 0x02;
  if is_but2() code = 0x04;
  if is_but3() code |= 0x08;

  return code;
}

/* --------------------------------------------------------------------
       Timer 0
   -------------------------------------------------------------------- */

/* ----------------------------------------------------------
   Timer0 - compare - Interrupt

   Tonerzeugung sowie Kontrolle (Multiplex) der Anzeigen-
   elemente
   ---------------------------------------------------------- */
ISR (TIM0_COMPA_vect)
{

  static uint8_t segmpx= 0;
  static uint8_t nfcmp = 0;
  static uint8_t lsflag = 0;

  // Tonerzeugung
  if (sound)
  {
    if (togglespk & 1)
    {
      setspk();
      togglespk= 0;
    }
    else
    {
      clrspk();
      togglespk= 1;
    }
  }

  // multiplexen der 7-Segmentanzeige
  if (segmpx== 0)                                              // MSD von globaler Variable digit2_value ausschieben
  {
    if (!(digit2_outmode))
      digit2_outbyte(led7sbmp[ digit2_value >> 4 ]);
    else
      digit2_outbyte(digit2_segvalues >> 8);
    mpx2_set();                                                // gemeinsame Kathode LED-Reihe auf 1 (und damit aus)
    mpx0_clr();                                                // MSD Display anzeigen
    digit2_stpuls();                                            // Inhalt Schieberegister ins Latch (und damit anzeigen)
  }

  if (segmpx== 1)                                              // LSD ausgeben
  {
    if (!(digit2_outmode))
      digit2_outbyte(led7sbmp[ digit2_value & 0x0f ]);
    else
      digit2_outbyte((digit2_segvalues & 0x00ff) );
    mpx0_set();                                                // MSD ausschalten
    mpx1_clr();                                                // LSD Display anzeigen
    digit2_stpuls();                                            // Inhalt Schieberegister ins Latch (und damit anzeigen)
  }
  if (segmpx== 2)
  {
    digit2_outbyte(led_anzbuf);
    mpx1_set();                                                // LSD ausschalten
    mpx2_clr();                                                // LED (gemeinsame Kathode) einschalten
    digit2_stpuls();                                            // Inhalt Schieberegister ins Latch (und damit anzeigen)
  }
  segmpx++;
  segmpx= segmpx % 3;

  TCNT0= 0;
}

// --------- Ende TIMERINTERRUPT --------------


/* ----------------------------------------------------------
   timer0_init

   initialisiert Timer0 fuer Interruptbetrieb. Innerhalb der
   ISR wird der Multiplexbetrieb der Anzeige und die Toner-
   zeugung vorgenommen
   ---------------------------------------------------------- */
void timer0_init(void)
{
  togglespk= 1;
  spk_init();                                                 // Anschlusspin des Lautsprechers als Ausgang schalten

  TCCR0B = 1 << CS02;
  OCR0A = (F_CPU/256) / startreload-1;
  TCNT0 = 0;

  TIMSK0 = 1 << OCIE0A;
  sei();
}

/* --------------------------------------------------------------------
       Ende Timer 0
   -------------------------------------------------------------------- */

/* --------------------------------------------------------------------
       "Sound" Funktionen
   -------------------------------------------------------------------- */

/* ----------------------------------------------------------
   settonfreq

   beschreibt das Compareregister A so, damit bei einge-
   schaltetem Ton die angewaehlte Frequenz erzeugt wird
   ---------------------------------------------------------- */
void settonfreq(uint16_t wert)
{
  OCR0A = (F_CPU/256) / wert-1;
}

/* ----------------------------------------------------------
   tonlen

   Verzoegerungsschleife fuer Spieldauer eines Tons
   ---------------------------------------------------------- */
void tonlen(int w)
{
  int cx;

  w = w*100;
  for (cx= 0; cx< w; cx++) { _delay_us(100); }
  sound= 0;
  _delay_ms(30);
}

/* ----------------------------------------------------------
   playnote

   Schaltet Ton mit der angegeben Note ein
   ---------------------------------------------------------- */
void playnote(char note)
{
  settonfreq(freqreload[note]);
  sound= 1;
}

/* ----------------------------------------------------------
   playstring

   spielt einen Notenstring ab
   ---------------------------------------------------------- */
void playstring(const unsigned char* const s)
{
  char ch;
  char aokt;
  int dind;

  aokt= 0; dind= 0;
  ch= pgm_read_byte(&(s[dind]));
  while (ch)
  {
    ch= pgm_read_byte(&(s[dind]));
    switch(ch)
    {
      case '-': { aokt= aokt-12; break; }
      case '+': { aokt= aokt+12; break; }
      case 'c': { playnote(aokt); break; }
      case 'C': { playnote(aokt+1); break; }
      case 'd': { playnote(aokt+2); break; }
      case 'D': { playnote(aokt+3); break; }
      case 'e': { playnote(aokt+4); break; }
      case 'f': { playnote(aokt+5); break; }
      case 'F': { playnote(aokt+6); break; }
      case 'g': { playnote(aokt+7); break; }
      case 'G': { playnote(aokt+8); break; }
      case 'a': { playnote(aokt+9); break; }
      case 'A': { playnote(aokt+10); break; }
      case 'h': { playnote(aokt+11); break; }
      case '1': { tonlen(16*playtempo); break; }
      case '2': { tonlen(8*playtempo); break; }
      case '3': { tonlen(6*playtempo); break; }
      case '4': { tonlen(4*playtempo); break; }
      case '5': { tonlen(3*playtempo); break; }
      case '8': { tonlen(2*playtempo); break; }
    }
    dind++;
  }
}
