/* ------------------------------------------------------------------
                             am433_transceiv.c

     Softwaremodul fuer Datenuebertragung mit sehr billigen
     AM 433 MHz Modulen (Sender / Empfaenger: Startstop-Oszillator
     und Pendelaudion)

     Die Software verwendet zur Uebertragung einfachstes Bitbanging
     nach RS232 Protokoll. Einzelne Bytes schicken ueber dieses
     Protokoll zuerst ein Zerobyte damit der Oszillator in Betrieb
     ist, danach ein Synchronisierungsbyte
                                0xaa
     welches kennzeichnet, dass, je nachdem ob ein String, ein
     Byte oder ein Integer uebertragen wird, alle nachfolgende
     Bytes Datenbyte(s) sind. Im Anschluss daran wird ein Check-
     summenbyte gesendet

     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz
     Fuses :  fuer 8 MHz intern
              lo 0xe2
              hi 0xdf


     22.11.2018  R. Seelig
   ------------------------------------------------------------------ */

#include "am433_transceiv.h"

/* --------------------------------------------------
                      swuart_putchar

     sendet ein Byte via RS232 Protokoll mittels
     Bitbanging

     Uebergabe:
                value : zusendender Wert
   -------------------------------------------------- */
void swuart_putchar(uint8_t value)
{
  char    i;
  uint8_t smask;

  cli();                                     // waehrend Uebertragung Interrupts sperren, die
                                             // ansonsten das Timing zerstoeren koennten
  txd_clr();
  _delay_us(bauddelay);
  smask= 0x01;
  for (i= 0; i< 8; i++)
  {
    if (value & smask)
    {
      txd_set();
    }
    else
    {
      txd_clr();
    }
    smask= smask << 1;
    _delay_us(bauddelay);
  }
  txd_set();
  _delay_us(bauddelay);
  sei();
}

/* --------------------------------------------------
                      swuart_getchar
   -------------------------------------------------- */
uint8_t swuart_getchar()
{
  uint8_t  value;
  char     i;
  uint8_t  smask;
  uint16_t timcnt;

  while ( rxd_isbit() );                     // auf das Startbit warten
  cli();                                     // waehrend Uebertragung Interrupts sperren, die
                                             // ansonsten das Timing zerstoeren koennten
  _delay_us(hbauddelay);
  _delay_us(bauddelay);

  value= 0;
  smask= 0x01;
  for (i= 0; i< 8; i++)
  {
    if (rxd_isbit())
    {
      value |= smask;
    }
    smask= smask << 1;
    _delay_us(bauddelay);
  }
  sei();
  return value;
}


/* --------------------------------------------------
                      am433_transmit

     sendet ein Byte ueber die 433 MHz Funkstrecke.
     Proprietaeres Protokoll:

        0 Byte senden (um Oszillator zu starten und
        Funkstrecke zu "beruhigen")

        0xaa synchronisiert Sender und Empfaenger

        Datenbyte

        Checksumm (0xaa + Datenbyte)

     Uebergabe:
                value : zusendender Wert
   -------------------------------------------------- */
void am433_transmit(uint8_t value)
{
  swuart_putchar(0x00);                 // Aktivitaet auf der Datenleitung erzeugen, damit
                                        // Oszillator bereits schwingt
  delay(delafterz);
  swuart_putchar(0xaa);                 // Synchronisationsbyte
  swuart_putchar(value);
  swuart_putchar(value+0xaa);
}

/* --------------------------------------------------
                      am433_transmitword

     sendet ein Byte ueber die 433 MHz Funkstrecke.
     Proprietaeres Protokoll:

        0 Byte senden (um Oszillator zu starten und
        Funkstrecke zu "beruhigen")

        0xaa synchronisiert Sender und Empfaenger

        Hi-Word
        Lo-Word

        Checksumm (0xaa + Datenbyte)

     Uebergabe:
                value : zusendender Wert
   -------------------------------------------------- */
void am433_transmitword(uint16_t value)
{
  uint8_t sum;

  sum= 0xaa;
  swuart_putchar(0x00);                 // Aktivitaet auf der Datenleitung erzeugen, damit
                                        // Oszillator bereits schwingt
  delay(delafterz);
  swuart_putchar(0xaa);                 // Synchronisationsbyte
  swuart_putchar(value >> 8);
  swuart_putchar(value & 0x00ff);
  sum += value >> 8;
  sum += value & 0x00ff;
  swuart_putchar(sum);
}

/* --------------------------------------------------
                     am433_transmitstr

     Sendet einen AsciiZ String auf der 433 MHz
     Funkstrecke. Es wird KEINE String Checksumme
     geprueft (sondern nur Checksumme einzelner
     Bytes)

     Uebergabe:
                string : zusendender Wert
   -------------------------------------------------- */
void am433_transmitstr(uint8_t *string)
{

  while (*string)
  {
    am433_transmit(*string++);
  }
  am433_transmit(0);
}

/* --------------------------------------------------
                     am433_transmitstr2


     Sendet einen AsciiZ String auf der 433 MHz
     Funkstrecke. Es wird eine String-Checksumme
     geprueft ermittelt und mitgesendet.

     Uebergabe:
                string : zusendender Wert
   -------------------------------------------------- */
void am433_transmitstr2(uint8_t *string)
{
  uint8_t sum;

  swuart_putchar(0x00);                 // Aktivitaet auf der Datenleitung erzeugen, damit
                                        // Oszillator bereits schwingt
  delay(delafterz);
  swuart_putchar(0xaa);                 // Synchronisationsbyte
  sum= 0xaa;
  while (*string)
  {
    swuart_putchar(*string);
    sum += *string;                     // Checksumme aufaddieren
    string++;
  }
  swuart_putchar(0);
  swuart_putchar(sum);                  // Checksumme senden
}

/* --------------------------------------------------
                      am433_receive

     wartet auf ein eingehendes Zeichen auf der
     Funkstrecke. Es erfolgt eine Checksummen-
     pruefung des Bytes

     Uebergabe:
          *errsum : wird zu 1 wenn Checksumme
                    nicht korrekt ist

     Rueckgabe:
          Empfangenes Zeichen auf der Funkstrecke
   -------------------------------------------------- */
uint8_t am433_receive(uint8_t *errsum)
{
  uint8_t amsync, ch, sum;

  while(1)                                // Endlosschleife, weil ausser dem
                                          // dem Synchronisationsbyte und dem Datenbyte
                                          // "atmosphaerische" Stoerungen Startbits
                                          // verursachen
  {
    amsync= swuart_getchar();

    if (amsync== 0xaa)
    {
      ch= swuart_getchar();
      sum= swuart_getchar();
      if (((ch + 0xaa) & 0x00ff)== sum) *errsum= 0; *errsum= 1;
      return ch;
    }
  }
}

/* --------------------------------------------------
                      am433_receiveword

     wartet auf einen eingehenden 16-Bit Integer  auf
     der Funkstrecke. Es erfolgt eine Checksummen-
     pruefung des Bytes

     Uebergabe:
          *errsum : wird zu 1 wenn Checksumme
                    nicht korrekt ist


     Rueckgabe:
          Empfangener Integer auf der Funkstrecke
   -------------------------------------------------- */
uint16_t am433_receiveword(uint8_t *errsum)
{
  uint8_t amsync, ch, sum, sum2;
  uint16_t cw;

  cw= 0; sum2= 0xaa;
  while(1)                                // Endlosschleife, weil ausser dem
                                          // dem Synchronisationsbyte und dem Datenbyte
                                          // "atmosphaerische" Stoerungen Startbits
                                          // verursachen
  {
    amsync= swuart_getchar();

    if (amsync== 0xaa)
    {
      ch= swuart_getchar();
      sum2 += ch;
      cw= ch; cw= cw << 8;
      ch= swuart_getchar();
      sum2 += ch;
      cw |= ch;
      sum= swuart_getchar();
      if (sum2== sum) *errsum= 0; else *errsum= 1;
      return cw;
    }
  }
}


/* --------------------------------------------------
                     am433_receivestr

      Empfaengt einen AsciiZ String auf der Funk-
      strecke. Es erfolgt KEINE Stringchecksummen-
      pruefung

      Parameter:

              *string : Zeiger auf einen Buffer-
                        speicher in den der String
                        eingelesen wird
   -------------------------------------------------- */
void am433_receivestr(uint8_t *string)
{
  uint8_t ch;
  uint8_t errsum;

  do
  {
    ch= am433_receive(&errsum);
    if ((ch== 0x0d) || (ch== 0x0a)) ch= 0;
    *string= ch;
    string++;
  } while(ch);
}

/* --------------------------------------------------
                      am433_receivestr2
      Empfaengt einen AsciiZ String auf der Funk-
      strecke. Es erfolgt eine Stringchecksummen-
      pruefung

      Parameter:

              *string : Zeiger auf einen Buffer-
                        speicher in den der String
                        eingelesen wird
   -------------------------------------------------- */
void am433_receivestr2(uint8_t *string)
{
  uint8_t amsync, ch, sum1, sum2;
  uint8_t *hptr;

  hptr= string;
  sum1= 0xaa;
  while(1)
  {
    amsync= swuart_getchar();
    if (amsync== 0xaa)
    {
      do
      {
        ch= swuart_getchar();
        sum1 += ch;
        *string= ch;
        string++;
      } while(ch);
      sum2= swuart_getchar();
      if (sum1 != sum2) *hptr= 0;       // fehlerhafte Checksumme, leeren String zurueckgeben
      return;
    }
  }
}
