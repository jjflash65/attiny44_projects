/* ------------------------------------------------------------------
                             tm16xx.c

     Header zum Ansprechen eines TM16xx, Treiberbaustein fuer
     4 und 6 stellige 7-Segmentanzeigen mit gemeinsamer Anode und
     zusaetzlichem Tasteninterface.

     Geeignet fuer: TM1651, TM1637

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf

     Pinbelegung

     ATtiny44     7-Segmentmodul
     ---------------------------
        PB1            CLK
        PB0            DIO

     CLK und DIO sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen

     10.01.2019 R. Seelig
   ------------------------------------------------------------------ */

/*

    Segmentbelegung der Anzeige:

        a
       ---
    f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  |  dp |
       ---               ---------------------------------------------------------------------------------
    e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
       ---
        d

*/

#include "tm16xx.h"

/* ----------------------------------------------------------
                     Globale Variable
   ---------------------------------------------------------- */

uint8_t    hellig    = 15;                // beinhaltet Wert fuer die Helligkeit (erlaubt: 0x00 .. 0x0f);

uint8_t    led7sbmp[16] =                // Bitmapmuster fuer Ziffern von 0 .. F
                { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71 };

  /* ----------------------------------------------------------
                           tm16_xofs
       Position, um die eine Anzeigenausgabe verschoben werden
       kann. Wird hier bspw. 2 angegeben, so erscheint die
       Ausgabe fuer Digit 0 nicht an Digit 0, sondern an
       Digit 2, Digit 1 an Digit 3 etc. Dieses dient vor allem
       dafuer, wenn ein 4-stelliger Wert auf einem 6-stelligen
       Display rechts buendig ausgegeben werden soll.
     ---------------------------------------------------------- */
int8_t    tm16_xofs = 0;


/*  ------------------- Kommunikation -----------------------

    Der Treiberbaustein tm16 wird etwas "merkwuerdig
    angesprochen. Er verwendet zur Kommunikation ein I2C
    Protokoll, jedoch OHNE eine Adressvergabe. Der Chip ist
    somit IMMER angesprochen. Aus diesem Grund wird die
    Kommunikation mittels Bitbanging vorgenommen. Hierfuer
    kann jeder freie I/O Anschluss des Controllers verwendet
    werden (siehe defines am Anfang).

    Ausserdem erfolgt im Gegensatz zu vielen andern I2C Bau-
    steinen die serielle Ausgabe mit dem niederwertigsten Bit
    (LSB first)
   ---------------------------------------------------------- */

void tm16_start(void)              // I2C Bus-Start
{
  bb_scl_hi();
  bb_sda_hi();
  puls_len();
  bb_sda_lo();
}

void tm16_stop(void)               // I2C Bus-Stop
{
  bb_scl_lo();
  puls_len();
  bb_sda_lo();
  puls_len();
  bb_scl_hi();
  puls_len();
  bb_sda_hi();
}

void tm16_write (uint8_t value)    // I2C Bus-Datentransfer
{
  uint8_t i;

  for (i = 0; i <8; i++)
  {
    bb_scl_lo();

    //  serielle Bitbangingausgabe, LSB first
    if (value & 0x01) { bb_sda_hi(); }
                   else { bb_sda_lo(); }
    puls_len();
    value = value >> 1;
    bb_scl_hi();
    puls_len();
  }
  bb_scl_lo();
  puls_len();                        // der Einfachheit wegen wird ACK nicht abgefragt
  bb_scl_hi();
  puls_len();
  bb_scl_lo();

}

/* -------------------------------------------------------
                    tm16_read(uint8_t ack)

   liest ein Byte vom I2c Bus.

   Uebergabe:
               1 : nach dem Lesen wird dem Slave ein
                   Acknowledge gesendet
               0 : es wird kein Acknowledge gesendet

   Rueckgabe:
               gelesenes Byte
   ------------------------------------------------------- */
uint8_t tm16_read(uint8_t ack)
{
  uint8_t data= 0x00;
  uint8_t i;

  bb_sda_hi();

  for(i= 0; i< 8; i++)
  {
    bb_scl_lo();
    puls_len();
    bb_scl_hi();

    puls_len();

    if(bb_is_sda()) data|= (1 << i);
  }

  bb_scl_lo();
  bb_sda_hi();

  puls_len();

  if (ack)
  {
    bb_sda_lo();
    puls_len();
  }

  bb_scl_hi();
  puls_len();

  bb_scl_lo();
  puls_len();

  bb_sda_hi();

  return data;
}


/*  ----------------------------------------------------------
                      Benutzerfunktionen
    ---------------------------------------------------------- */


 /*  ------------------- SELECTPOS ---------------------------

        waehlt die zu beschreibende Anzeigeposition aus
     --------------------------------------------------------- */
void tm16_selectpos(int8_t nr)
{
  tm16_start();
  tm16_write(0x40);                // Auswahl LED-Register
  tm16_stop();

  tm16_start();
  nr+= tm16_xofs;
  tm16_write(0xc0 | nr);           // Auswahl der 7-Segmentanzeige
}

/*  ----------------------- SETBRIGHT ------------------------

       setzt die Helligkeit der Anzeige
       erlaubte Werte fuer Value sind 0 .. 7
       Wert 8 fuer Anzeige aus !
    ---------------------------------------------------------- */
void tm16_setbright(uint8_t value)
{
  hellig= (value % 8) + 8;
}

/*  ------------------------- CLEAR -------------------------

       loescht die Anzeige auf dem Modul
    --------------------------------------------------------- */
void tm16_clear(void)
{
  uint8_t i;

  tm16_selectpos(0);
  for(i=0; i<6; i++) { tm16_write(0x00); }
  tm16_stop();

  tm16_start();
  tm16_write(0x80 | hellig);        // unteres Nibble beinhaltet Helligkeitswert
  tm16_stop();

}

/*  ---------------------- SETBMP ---------------------------
       gibt ein Bitmapmuster an einer Position aus
    --------------------------------------------------------- */
void tm16_setbmp(uint8_t pos, uint8_t value)
{
  tm16_selectpos(pos);             // zu beschreibende Anzeige waehlen

  tm16_write(value);               // Bitmuster value auf 7-Segmentanzeige ausgeben
  tm16_stop();

}

/*  ---------------------- SETZIF ---------------------------
       gibt ein Ziffer an einer Position aus
       Anmerkung: das Bitmuster der Ziffern ist in
                  led7sbmp definiert
    --------------------------------------------------------- */
void tm16_setzif(uint8_t pos, uint8_t zif)
{
  tm16_selectpos(pos);             // zu beschreibende Anzeige waehlen

  zif= led7sbmp[zif];
  tm16_write(zif);               // Bitmuster value auf 7-Segmentanzeige ausgeben
  tm16_stop();

}
/*  ----------------------- SETSEG --------------------------
       setzt ein einzelnes Segment einer Anzeige

       pos: Anzeigeposition (0..3)
       seg: das einzelne Segment (0..7 siehe oben)
    --------------------------------------------------------- */
void tm16_setseg(uint8_t pos, uint8_t seg)
{

  tm16_selectpos(pos);             // zu beschreibende Anzeige waehlen
  tm16_write(1 << seg);
  tm16_stop();

}

/*  ----------------------- SETDEZ --------------------------
       gibt einen 4-stelligen dezimalen Wert auf der
       Anzeige aus

       Uebergabe:

         value : auszugebender Wert
         dpanz : Position der Anzeige Dezimalpunkt, 1 ent-
                 spricht Anzeige rechts (und somit keine
                 Nachkommastelle)
                 0 => es erfolgt keine Anzeige
    --------------------------------------------------------- */
void tm16_setdez(int value, uint8_t dpanz)
{
  uint8_t i,v, bmp;

  for (i= 4; i> 0; i--)
  {
    v= value % 10;
    bmp= led7sbmp[v];
    if (dpanz== 5-i) bmp= bmp | 0x80;     // Dezimalpunkt setzen
    tm16_setbmp(i-1, bmp);
    value= value / 10;
  }
}

#if (enable_6dezdigit == 1)

  /*  -------------------- SETDEZ6DIGIT -----------------------
         gibt einen 6-stelligen dezimalen Wert auf der
         Anzeige aus

         Uebergabe:

           value : auszugebender Wert
           dpanz : Position der Anzeige Dezimalpunkt, 1 ent-
                   spricht Anzeige rechts (und somit keine
                   Nachkommastelle)
                   0 => es erfolgt keine Anzeige
      --------------------------------------------------------- */
  void tm16_setdez6digit(uint32_t value, uint8_t dpanz)
  {
    uint8_t i,v, bmp;

    for (i= 6; i> 0; i--)
    {
      v= value % 10;
      bmp= led7sbmp[v];
      if (dpanz== 7-i) bmp= bmp | 0x80;     // Dezimalpunkt setzen
      tm16_setbmp(i-1, bmp);
      value= value / 10;
    }
  }
#endif


#if (enable_6dezdigit_nonull == 1)

  /* --------------------SETDEZ6DIGIT_NONULL ------------------
       zeigt eine max. 6 stellige dezimale Zahl OHNE fuehrende
       Null an.

           Uebergabe:

             value : auszugebender Wert
             dpanz : Position der Anzeige Dezimalpunkt, 1 ent-
                     spricht Anzeige rechts (und somit keine
                     Nachkommastelle)
                     0 => es erfolgt keine Dezimalpunktanzeige
     --------------------------------------------------------- */
  void tm16_setdez6digit_nonull(int32_t value, uint8_t dpanz)
  {
    uint8_t  i,v, bmp, first;
    uint32_t teiler;
    uint8_t  negflag;

    teiler= 100000;
    first= 1;
    negflag= 0;
    if (value< 0)
    {
      value= -value;
      tm16_setseg(0,6);
      negflag++;
    }

    for (i= 1; i< 7; i++)
    {
      v= value / teiler;
      value= value - (v * teiler);
      teiler= teiler / 10;

      bmp= led7sbmp[v];
      if (dpanz== 7-i) bmp= bmp | 0x80;     // Dezimalpunkt setzen

      if (!((v== 0) && (first) && (i != 6)))
      {
        tm16_setbmp(i-1, bmp);
        first= 0;
      }
      else
      {
        // fuehrende Null nicht anzeigen
        if (!((i==1) && (negflag)))
        {
          if (dpanz== 7-i)
          {
            tm16_setbmp(i-1, 0x80);
          }
          else
          {
            tm16_setbmp(i-1, 0);
          }
        }
      }
    }
  }

#endif

/*  ----------------------- SETHEX --------------------------
       gibt einen 4-stelligen hexadezimalen Wert auf der
       Anzeige aus
    --------------------------------------------------------- */
void tm16_sethex(uint16_t value)
{
  uint8_t i,v;

  for (i= 4; i> 0; i--)
  {
    v= value % 0x10;
    tm16_setbmp(i-1, led7sbmp[v]);
    value= value / 0x10;
  }
}

#if (enable_hex6digit == 1)

  /*  -------------------- SETHEX6DIGIT -----------------------
         gibt einen 6-stelligen hexadezimalen Wert auf der
         Anzeige aus
      --------------------------------------------------------- */
  void tm16_sethex6digit(uint32_t value)
  {
    uint8_t i,v;

    for (i= 6; i> 0; i--)
    {
      v= value % 0x10;
      tm16_setbmp(i-1, led7sbmp[v]);
      value= value / 0x10;
    }
  }

#endif

/*  ----------------------- READKEY -------------------------
      liest angeschlossene Tasten ein und gibt dieses als
      Argument zurueck.

      Anmerkung:
        Es wird keine Tastenmatrix zurueck geliefert. Ist
        mehr als eine Taste aktiviert, wird nur die hoechste
        Taste zurueck geliefert. Somit ist es nicht moeglich
        mehrere Tasten gleichzeitig zu betaetigen.

    --------------------------------------------------------- */
uint8_t tm16_readkey(void)
{
  uint8_t key;

  key= 0;
  tm16_start();
  tm16_write(0x42);
  key= ~tm16_read(1);
  tm16_stop();
  if (key) key -= 7;
  return key;
}

/*  ----------------------- INIT ----------------------------
       initialisiert die Anschluesse des Controllers zur
       Kommunikation als Ausganege und loescht die Anzeige
    ---------------------------------------------------------- */
void tm16_init(void)
{
  scl_init();
  sda_init();
  tm16_clear();
}
