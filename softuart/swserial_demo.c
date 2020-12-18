/* -----------------------------------------------
                  swserial_demo.c

     130.11.2018
   ----------------------------------------------- */


#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "softuart.h"
#include "my_printf.h"

#define printf    my_printf

int   laenge, breite, hoehe;
char  ch;

/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  uartsw_putchar(ch);
}

/* --------------------------------------------------
                       CHECKINT16
      testet, ob eine String im Integerzahlenbereich
      liegt und wenn dem so ist, wird der String in
      einen Integer gewandelt.

      Uebergabe:
              *p    : Zeiger auf einen String
              *myi  : Zeiger auf den zurueck zu
                      gebenden Integer

      Rueckgabe:
              0     : wenn nicht im Zahlenbereich
              1     : wenn im Zahlenbereich
   -------------------------------------------------- */

int checkint16(char *p, int *myi)
{
  char     neg,l;
  char     *p2;
  int      x;
  int32_t  z,z2;

  if ( *p == 0 )
  {
    *myi= 0;
    return 1;
  }
  p2= p;
  l= 0;
  while ( *p++) { l++; }          // ermittelt die Laenge des Strings
  p= p2;
  neg= 0;
  if ( *p == '-')                  // negative Zahl ?
  {
    neg= 1;
    p2++; p++;                   // Zahlenbereich nach dem Minuszeichen
    l--;                         // und Zahl ist eine Stelle kuerzer
  }
  p += (l-1);                    // Pointer auf die Einerzahlen setzen
  z= *p - 48;                    // nach Ziffer umrechnen
  z2= 1;
  if (l> 1)
  {
    l--;                         // Laenge "kuerzen" weil 10er Stelle
    p--;                         // Zeiger auf die 10er Stelle
    do
    {
      z2= z2*10;
      z= z+ (z2 * ( *p - 48 ));  // Ziffer * Multiplikator + alten Inhalt von z
      p--;
      l--;
    } while (l);                 // fuer alle Stellen wiederholen
  }
  if (((z> 32767) && (!neg)) | (z> 32768))
  {
    *myi= 0;
    return 0;
  }
  if (neg) { z= z * -1; }
  x= z;
  *myi= z;
  return 1;
}

/* --------------------------------------------------
                     UART_READINT
     liest einen Integerzahlenwert ueber die RS-232
     ein.
     Eine Ueberpruefung auf den Integerwertebereich
     findet statt !!!!
   -------------------------------------------------- */

signed int uart_readint()
{

  #define maxinlen   7         // max. 6 Zeichen + NULL

  char str[maxinlen];
  char *p;
  char *pz;
  char  ch, cnt, b;
  signed int i;


  p= &str[0];                  // p zeigt auf die Adresse von str
  pz= p;                       // pz zeigt immer auf erstes Zeichen im String
  *p = 0;                      // und setzt dort NULL Byte
  cnt= 0;
  do
  {
    do
    {
      ch= uartsw_getchar();
      if ((ch>= '0') && (ch<= '9'))
      {
        if (cnt < maxinlen-1)
        {
          *p++= ch;              // schreibt Char in den String und erhoeht Pointer
          *p= 0;                 // und schreibt neues NULL (Endekennung)
          cnt++;
          uartsw_putchar(ch);      // Echo des eingebenen Zeichens
        }
      }
      switch (ch)
      {
        case '-' :
          {
            if (cnt == 0)
            {
              *p++= ch;
              *p= 0;
              cnt++;
              uartsw_putchar(ch);
            }
            break;
          }
        case 8   :
          {
            if (cnt> 0)
            {
              uartsw_putchar(ch);
              p--;
              *p= 0;
              cnt--;
            }
            break;
          }
        default  :
          {
            break;
          };
      }
    } while (ch != 0x0d);        // wiederholen bis Returnzeichen eintrifft
    b=  (checkint16( &str[0], &i ));
    if (!b)
    {
      for (i= cnt; i> 0; i--)
      {
        uartsw_putchar(8);         // gemachte, fehlerhafte Eingabe durch Backspace
                                 // loeschen
      }
      cnt= 0;                    // und Zaehler zuruecksetzen
      p= pz;
      *p= 0;
    }
  } while (!b);      // Zahl soll im 16 Bit Integerbereich liegen
  return i;
}

/* --------------------------------------------------
                       UART_PUTINT
     gibt einen Integerzahlenwert ueber die RS-232
     aus.
   -------------------------------------------------- */

void uart_putint(int i)
{
  int  z,z2;
  char b, first;

  first= 1;
  if (!i) {uartsw_putchar('0'); return;}
  if (i< 0) { uartsw_putchar('-'); }
  i= abs(i);
  z= 10000;
  for (b= 5; b; b--)
  {
    z2= i / z;
    if (! ((first) && (z2 == 0)) )
    {
      uartsw_putchar(z2 + 48);
      first= 0;
    }
    z2 = z2 * z;
    i -= z2;
    z = z / 10;
  }
}



int main(void)
{
    char myzahlen[30] = "13.89 #14.92 #15.97";
    char *ptr;

  uartsw_init();

    printf("Extraktion einer Zahlenreihe\n\r\n\r");

    /* strtok platziert ein 0-Byte an der Stelle
      an der das Token gefunden wurde. ptr ist nun ein
      Zeiger auf einen String der an der Stelle
      terminiert ist, an der das Token WAR                */

    ptr = strtok(myzahlen, "#");

    while (ptr)                // solange ptr nicht auf ein
    {                          // Textende zeigt

      if (ptr)  printf("%s\n\r", ptr);

      /* Ein weiterer Aufruf von strtok mit einem NULL
         Zeiger sucht das naechste Token ersetzt dieses,
         uebergibt wieder den Zeiger darauf */
      ptr = strtok(NULL, "#");
    }


  while (1)
  {
    printf("\n\n\rAVR 19200 Bd");
    printf("\n\rQuaderberechnung\n\r");
    printf("\n\rLaenge des Quaders: ");
    laenge= uart_readint();
    printf("\n\rBreite des Quaders: ");
    breite= uart_readint();
    printf("\n\rHoehe des Quaders : ");
    hoehe= uart_readint();
    printf("\n\rQuaderoberflaeche: ");
    uart_putint(2 * (laenge*breite +     \
       laenge*hoehe + breite*hoehe));
    ch= uartsw_getchar();
    uartsw_crlf();
  }
}
