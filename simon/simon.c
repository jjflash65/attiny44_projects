/* ----------------------------------------------------------
                            simon.c


     ATtiny44 Umsetzung des Spiels "Simon sagt" mit LED und
     zweistelliger 7-Segmentanzeige

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     zusaetzliche Hardware:
                 - zweistellige Siebensegmentanzeige mit
                   gemeinsamer Kathode
                 - 4 LED
                 - 4 Taster
                 - Lautsprecher

     Anschlussbelegungen siehe mini_io.h

     30.10.2018  R. Seelig

   ---------------------------------------------------------- */

#include "mini_io.h"


#define sndontime       310             // Wiedergabedelay bei Computerabspielsequenz
#define sndofftime      50              // Auszeit beim Abspielen
#define keydelay        60              // Tastenentprellung

#define delay           _delay_ms

static const unsigned char jingle01[] PROGMEM = { "g8g8g8h8o+e4d3" };
static const unsigned char schneew [] PROGMEM = { "c4d4e2g4e2g4e1d4e4f2g4f2g4f1g4a4h2+f4-h2+f4-h1a4h4+c2e4c2-a4g1e4g4+c1-h1+d1c1-a2+c4-a2+c4-a1" };

// Frequenzen der 4 Spieltoene
const uint16_t ton_nr[4] = { 210, 440, 700, 1000 };



/* ----------------------------------------------------------
     getsoundkey

     liest Tasten ein und spielt beim Druecken einer Taste
     einen Ton mit der in ton_nr angegebenen Frequenz ab.
   ---------------------------------------------------------- */
uint8_t getsoundkey(void)
{
  while(1)
  {
    if (is_but0())
    {
      settonfreq(ton_nr[0]);
      sound= 1;
      led_anzbuf= 0x01;
      delay(keydelay);
      while(is_but0()) sound= 1;
      sound= 0;
      delay(keydelay);
      led_anzbuf= 0;
      settonfreq(ton_nr[0]);               // Multiplexfrequenz auf niedrigsten Ton
      return 0;
    }

    if (is_but1())
    {
      settonfreq(ton_nr[1]);
      sound= 1;
      led_anzbuf= 0x02;
      delay(keydelay);
      while(is_but1()) sound= 1;
      sound= 0;
      delay(keydelay);
      led_anzbuf= 0;
      settonfreq(ton_nr[0]);
      return 1;
    }

    if (is_but2())
    {
      settonfreq(ton_nr[2]);
      sound= 1;
      led_anzbuf= 0x04;
      delay(keydelay);
      while(is_but2()) sound= 1;
      sound= 0;
      delay(keydelay);
      led_anzbuf= 0;
      settonfreq(ton_nr[0]);
      return 2;
    }

    if (is_but3())
    {
      settonfreq(ton_nr[3]);
      sound= 1;
      led_anzbuf= 0x08;
      delay(keydelay);
      while(is_but3()) sound= 1;
      sound= 0;
      delay(keydelay);
      led_anzbuf= 0;
      settonfreq(ton_nr[0]);
      return 3;
    }
  }
}

/* ----------------------------------------------------------
     random16_bit

     liefert 16-Bit grosse Pseudozufallszahl
   ---------------------------------------------------------- */
uint16_t random_16bit(void)
{
  static uint16_t lsb;
  static uint16_t lfsr = 1;
  static uint16_t anz  = 0;

  lsb = lfsr & 1;                        // niederwertigstes Bit des Schieberegisters
  lfsr >>= 1;                            // das Schieberegister, eine Stelle nach rechts
  if (lsb) { lfsr ^= 0xb400; }           // wenn LSB gesetzt, XOR-Togglemaske (B400h)auf SR anwenden

  if (lfsr== 18493)                      // Stelle, an der die 0 eingefuegt wird (willkuerlich)
  {
    anz++;
   return 0;
  }
  anz++;
  return lfsr;
}

/* ----------------------------------------------------------
     random_16_bit_init

     initialisiert den Zufallszahlengenerator.

     Uebergabe:
        startwert: Startwert, ab der der Zufallszahlen-
                   generator durch Verschieben die Zahlen
                   generiert
   ---------------------------------------------------------- */
void random_16bit_init(uint16_t startwert)
{
  uint16_t p;

  for (p= 0; p< startwert; p++) random_16bit();
}

/* ----------------------------------------------------------
     spielt den "Verlierersound" ab
   ---------------------------------------------------------- */
void loosesound(void)
{
  uint16_t i, i2;

  sound= 1;
  for (i= 450; i> 120; i--)
  {
    sound= 1;
    settonfreq(i);

    for (i2= 0; i2< (i / 100); i2++)
      delay(1);
    sound= 0;
  }
  settonfreq(210);
}

/* ----------------------------------------------------------
     seqcomp

     Vergleicht 2 Arrays auf Gleichheit.

     Uebergabe:
        s1  : Zeiger auf erstes Array
        s2  : Zeiger auf zweites Array
        anz : Anzahl zu vergleichender Werte

      Rueckgabe:
          1 : s1 = s2
          0 : s1 != s2
   ---------------------------------------------------------- */
uint8_t seqcomp(uint8_t *s1, uint8_t *s2, uint8_t anz)
// liefert 1 bei Gleichheit, 0 bei Ungleichheit
{
  uint8_t i;

  for (i= 0; i< anz; i++)
  {
    if (*s1 != *s2) return 0;
    s1++;
    s2++;
  }
  return 1;
}



/* ----------------------------------------------------------
                             M-A-I-N
   ---------------------------------------------------------- */
int main(void)
{
  uint8_t  userseq[64];                   // Pufferspeicher der durch Spieler eingegebenen Sequenz
  uint8_t  compseq[64];                   // Pufferspeicher der vom Computer vorgegebenen, zu
                                          // wiederholenden Frequenz
  uint8_t  uind = 0;
  uint8_t  cind = 0;

  uint8_t  b, b1, b2;
  uint8_t  f, i, i2;
  uint16_t startwert;
  uint8_t  gamestat = 0;

  timer0_init();
  digit2_init();

  button_init();

  digit2_segvalues= 0;
  digit2_outmode= 1;
  b1= 0x02; b2= 0x20;
  for (i2= 0; i2< 5; i2++)
  {
    for (i= 0; i< 6; i++)
    {
      digit2_segvalues= (b1 << 8) | b2;
      b1= b1 >> 1;
      if (b1== 0x00) b1= 0x20;
      b2= b2 << 1;
      if (b2== 0x40) b2= 0x01;
      delay(70);
    }
  }
  playstring(&jingle01[0]);
  digit2_outmode= 0;
  digit2_dezout(0);

  while(1)
  {
    // LEDS blinken lassen als Erkennung eines neuen Spiels
    for (i= 0; i< 8; i++)
    {
      led_anzbuf= 0;
      delay(80);
      led_anzbuf= 0x0f;
      delay(80);
    }
    startwert= 0;
    led_anzbuf= 0x0f;                  // alle LEDs an (Benutzeraufforderung fuer rechten Button)
    gamestat= 0;
    while (!(is_but0()))            // Startwert fuer Pseudozufallszahl
    {
      startwert++;
    }
    random_16bit_init(startwert);

    delay(keydelay);
    while( (is_but0()) );

    delay(keydelay);
    led_anzbuf= 0;

    cind= 0;
    digit2_dezout(cind);
    do
    {
      b= random_16bit();
      b= b % 4;
      compseq[cind]= b;
      cind++;

      // Abspielen der Computersequenz
      for (i= 0; i< cind; i++)
      {
        settonfreq(ton_nr[compseq[i]]);
        switch (compseq[i])
        {
          case 0 :
              {
                led_anzbuf= 0x01;
                break;
              }
          case 1 :
              {
                led_anzbuf= 0x02;
                break;
              }
          case 2 :
              {
                led_anzbuf= 0x04;
                break;
              }
          case 3 :
              {
                led_anzbuf= 0x08;
                break;
              }
        }
        sound= 1;
        delay(sndontime);
        sound= 0;
        led_anzbuf= 0;
        delay(sndofftime);
      }

      uind= 0;
      for (i= 0; (i< cind) && (gamestat< 1); i++)
      {
        b= getsoundkey();
        userseq[uind]= b;
        uind++;
        f= seqcomp(&compseq[0], &userseq[0], uind);
        if (!f)
        {
          loosesound();
          delay(200);
          playstring(&jingle01[0]);
          gamestat= 1;
        }
        if (cind == 63) gamestat= 1;      // okay, eigentlich hat man gewonnen
      }
      if (f) digit2_dezout(cind);

      delay(600);
    } while (!(gamestat));
  }
}



