/* -------------------------------------------------------
                         rda5807_test.c

     Testprogramm fuer das UKW-Modul RDA5807M  mit
     I2C Interface. "Bedienung" ueber die serielle
     Schnittstelle.

     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz

     11.10.2018  R. Seelig

     Hinweis:
        um die Datenuebertragung von ATtiny44 und PC
        zu gewaehrleisten wurde die Baudrate der seriellen
        Uebertragung auf 4800 Baud gesenkt (und deshalb)
        gibt es im Projektverzeichnis ein gesondertes
        usiuart.h (in dem die Baudrate definiert ist)

        Bei der "Bedienung" ueber ein Terminalprogramm
        (wie vorgesehen) kann es zu Fehlfunktionen
        kommen, wenn eine Taste dauernd gedrueckt bleibt
        (wie bspw. fuer Sender hoch/runter oder Laut-
        staerkeeinstellung).

        ==> Das hier ist ein Testprogramm fuer den
        RDA5807 Chip und nicht als komplette Radiofirm-
        ware gedacht.

        Ein komplette Firmware fuer ein Nachbauprojekt
        findet sich in rda5807_display.c


   ------------------------------------------------------ */

/* ------------------------------------------------------

   Pinbelegung RDA5807 Modul

                                 RDA5807-Modul
                                --------------
                           SDA  |  1  +-+ 10  |  Antenne
                           SCL  |  2  +-+  9  |  GND
                           GND  |  3       8  |  NF rechts
                            NC  |  4 _____ 7  |  NF links
                         +3.3V  |  5 _____ 6  |  GND
                                 -------------

   ------------------------------------------------------ */

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "usiuart.h"
#include "rda5807.h"
#include "my_printf.h"

#define printf       my_printf  // umleiten von my_printf nach printf
#define delay        _delay_ms


/* --------------------------------------------------
     my_putchar

     Diese Funktion wird von < my_printf > zur Ausgabe
     von Zeichen aufgerufen. Hier ist eine Hardware-
     zeichenausgabefunktion anzugeben, mit der
     my_printf zusammen arbeiten soll.
   -------------------------------------------------- */
void my_putchar(char ch)
{
  uart_putchar(ch);
}

/* --------------------------------------------------
     show_tune

     zeigt die Empfangsfrequenz und die aktuell
     eingestellte Lautstaerke an
   -------------------------------------------------- */
void show_tune(void)
{
  char i;

  if (aktfreq < 1000) { my_putchar(' '); }
  printf("  %k MHz  |  Volume: ",aktfreq);

  if (aktvol)
  {
    my_putchar('0');
    for (i= 0; i< aktvol-1; i++) { my_putchar('-'); }
    my_putchar('x');
    i= aktvol;
    while (i< 15)
    {
      my_putchar('-');
      i++;
    }
  }
  else
  {
    printf("x--------------");
  }
  printf("  \r");
}

/* --------------------------------------------------
     setnewtune

     setzt eine neue Empfangsfrequenz und zeigt die
     neue Frequenz an.
   -------------------------------------------------- */
void setnewtune(uint16_t channel)
{
  aktfreq= channel;
  rda5807_setfreq(aktfreq);
  show_tune();
}

/* --------------------------------------------------
      rda5807_scandown

     Sendersuchlauf in Richtung untere Frequenz
   -------------------------------------------------- */
void rda5807_scandown(void)
{
  uint8_t siglev;

  rda5807_setvol(0);

  if (aktfreq== fbandmin) { aktfreq= fbandmax; }
  do
  {
    aktfreq--;
    setnewtune(aktfreq);
    siglev= rda5807_getsig();
    if (aktfreq < 1000) { printf(" "); }
    printf("  %k MHz  \r",aktfreq, siglev);
  }while ((siglev < sigschwelle) && (aktfreq > fbandmin));

  rda5807_setvol(aktvol);

}


/* --------------------------------------------------
      rda5807_scanup

     Sendersuchlauf in Richtung obere Frequenz
   -------------------------------------------------- */
void rda5807_scanup(void)
{
  uint8_t siglev;

  rda5807_setvol(0);

  if (aktfreq== fbandmax) { aktfreq= fbandmin; }
  do
  {
    aktfreq++;
    setnewtune(aktfreq);
    siglev= rda5807_getsig();
    if (aktfreq < 1000) { printf(" "); }
    printf("  %k MHz  \r",aktfreq, siglev);
  }while ((siglev < sigschwelle) && (aktfreq < fbandmax));

  rda5807_setvol(aktvol);

}


/* ---------------------------------------------------------------------
                                   MAIN
   --------------------------------------------------------------------- */
int main(void)
{
  char ch;

  printfkomma= 1;                       // my_printf verwendet mit Formatter %k eine Kommastelle

  i2c_master_init();
  rda5807_init();
  uart_init(19200);

  printf("\n\n\r  ----------------------------------\n\r");
  printf(      "    UKW-Radio mit I2C-Chip RDA5807\n\r");
  printf(      "  ----------------------------------\n\n\r");

  printf(      "      (+)     Volume+\n\r");
  printf(      "      (-)     Volume-\n\n\r");
  printf(      "      (u)     Empfangsfrequenz hoch\n\r");
  printf(      "      (d)     Empfangsfrequenz runter\n\n\r");
  printf(      "      (a)     Sendersuchlauf hoch\n\r");
  printf(      "      (s)     Sendersuchlauf runter\n\n\r");
  printf(      "      (1..6)  Stationstaste\n\n\r");

  show_tune();
  while (uart_ischar()) { ch = uart_getchar(); }

  while(1)
  {
    ch= 0;
    ch= uart_getchar();
    switch (ch)
    {
      case '+' :                            // Volume erhoehen
        {
          if (aktvol< 15)
          {
            aktvol++;
            rda5807_setvol(aktvol);
            my_putchar('\r');
            show_tune();
          }
          break;
        }

      case '-' :
        {
          if (aktvol> 0)                      // Volume verringern
          {
            aktvol--;
            rda5807_setvol(aktvol);
            my_putchar('\r');
            show_tune();
          }
          break;
        }

      case 'd' :                           // Empfangsfrequenz nach unten
        {
          if (aktfreq > fbandmin)
          {
            aktfreq--;
            setnewtune(aktfreq);
            show_tune();
          }
        break;
        }
      case 'u' :                           // Empfangsfrequenz nach oben
        {
          if (aktfreq < fbandmax)
          {
            aktfreq++;
            setnewtune(aktfreq);
            show_tune();
          }
          break;
        }

      case 's' :                           // Suchlauf nach unten
        {
          rda5807_scandown();
          show_tune();

          break;
        }

      case 'a' :                         // Suchlauf nach oben
        {
          rda5807_scanup();
          show_tune();

          break;
        }

      default  : break;
    }

    if ((ch >= '1') && (ch <= '6'))
    {
      setnewtune(festfreq[ch-'0'-1]);
      show_tune();
    }

  }
}
