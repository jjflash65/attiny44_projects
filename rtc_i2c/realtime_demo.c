/* ----------------------------------------------------------
                      realtime_demo.c

     Demoprogramm fuer DS1307 / DS3231  RTC-Baustein

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL

     02.01.2019

   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"

#include "i2c_sw.h"
#include "rtc_i2c.h"
#include "oled1306_i2c.h"


#define autospeed       50                       // Geschwindigkeit in Millisekunden, mit der beim Stellen
                                                 // der Uhr Werte inkrementiert / dekrementiert werden
#define autoenter       24                       // Anzahl Zyklen, bis das automatische Zaehlen beim Stellen
                                                 // aktiviert wird


/* --------------------------------------------------
              Pinbelegung der Tasten:

     PA5: Aktivierung fuer Uhr stellen
     PA7: Taste fuer inkrementieren
     PA6: Taste fuer dekrementieren
   -------------------------------------------------- */
// Button "Uhr stellen"
#define buts_init()     { PA5_input_init(); PA5_set(); }
#define is_buts()       ( !(is_PA5() & 0x01) )

// Button "plus"
#define butp_init()     { PA7_input_init(); PA7_set(); }
#define is_butp()       ( !(is_PA7() & 0x01) )

// Button "minus"
#define butm_init()     { PA6_input_init(); PA6_set(); }
#define is_butm()       ( !(is_PA6() & 0x01) )

#define button_init()   { buts_init(); butp_init(); butm_init(); }

#define prints(tx)      (oled_putromstring(PSTR(tx)))       // Anzeige String aus Flashrom: prints("Text");
#define delay           _delay_ms


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  oled_putchar(ch);
}

/* -------------------------------------------------------
     oled_putromstring

     gibt einen Text, der im Flashrom gespeichert ist
     auf dem LC-Display aus (Wird vom Macro <prints>
     aufgerufen .

     Bsp.:

       static const uint8_t mytext[] PROGMEM = "Text";

       oled_putromstring(PSTR("H. Welt"));
       oled_putromstring(&mytext[0]);

       prints("Hallo");

   ------------------------------------------------------- */
void oled_putromstring(const uint8_t *dataPtr)
{
  unsigned char c;

  for (c=pgm_read_byte(dataPtr); c; ++dataPtr, c=pgm_read_byte(dataPtr))
    my_putchar(c);
}

/* -------------------------------------------------------
     putdez

     gibt eine 2-stellige Dezimalzahl an der aktuellen
     Displayposition aus
   ------------------------------------------------------- */
void put2dez(uint8_t value)
{
  uint8_t b;

  b= value / 10;
  my_putchar(b + '0');
  value -= (b*10);
  my_putchar(value + '0');
}

/* --------------------------------------------------
     hexnibbleout

     gibt die unteren 4 Bits eines chars als Hexa-
     ziffer aus. Eine Pruefung ob die oberen vier
     Bits geloescht sind erfolgt NICHT !
  --------------------------------------------------  */
void hexnibbleout(uint8_t b)
{
  if (b< 10) b+= '0'; else b+= 55;
  my_putchar(b);
}

/* --------------------------------------------------
      puthex

      gibt einen uint8_t hexadezimal aus.
   -------------------------------------------------- */
void puthex(uint8_t h)
{
  uint8_t b;

  b= h;
  b= (h >> 4) & 0x0f;
  hexnibbleout(b);
  b= h & 0x0f;
  hexnibbleout(b);
}

/* --------------------------------------------------
      dez2bcd

      wandelt eine dezimale Zahl in eine BCD
      Bsp: value = 45
      Rueckgabe    0x45
   -------------------------------------------------- */
uint8_t dez2bcd(uint8_t value)
{
  uint8_t hiz,loz,c;

  hiz= value / 10;
  loz= (value -(hiz*10));
  c= (hiz << 4) | loz;
  return c;
}

/* --------------------------------------------------
     clock_setscreen

     Zeigt eine Displaymaske zum Einstellen der
     Uhr an.
   -------------------------------------------------- */
void clock_setscreen(void)
{
  clrscr();
  prints("Tag    :\n\r");
  prints("Monat  :\n\r");
  prints("Jahr   :\n\n\r");
  prints("Stunde :\n\r");
  prints("Minute :\n\r");
  prints("Sekunde:\n\r");

}

/* --------------------------------------------------
      dezval_add

      Erhoeht den Wert in < startval > um eins,
      gibt diesen Wert auf dem Display aus und gibt
      diesen als Argument zurueck
   -------------------------------------------------- */
uint8_t dezval_add(uint8_t x, uint8_t y, uint8_t startval, uint8_t divmod, uint8_t minanz)
{
  startval++;
  startval= startval % divmod;
  if (startval< minanz) startval= minanz;
  gotoxy(x,y); put2dez(startval);
  return startval;
}

/* --------------------------------------------------
      dezval_dec

      Vermindert den Wert in < startval > um eins,
      gibt diesen Wert auf dem Display aus und gibt
      diesen als Argument zurueck
   -------------------------------------------------- */
uint8_t dezval_dec(uint8_t x, uint8_t y, uint8_t startval, uint8_t divmod, uint8_t minanz)
{
  if (startval> minanz) startval--; else startval= divmod-1;
  gotoxy(x,y); put2dez(startval);
  return startval;
}

/* --------------------------------------------------
      dezval_read

      liest ueber 3 Tasten eine 2-stellige Dezimal-
      zahl ein. Die Eingabe wird auf dem Display
      angezeigt. Beim laengeren druecken einer Ein-
      gabetaste erfolgt ein schnelles Durchzaehlen
      innerhalb des gewaehlten Wertebereichs

      Parameter:

      x,y      : Koordinaten, an der die Anzeige
                 erfolgt
      startval : Vorgabezahl, bei der die Eingabe
                 startet
      divmod   : Modulo Diviso, hierdurch erfolgt
                 eine Vorgabe, ab wann die Ein-
                 gabe ueberlaueft und auf den Mini-
                 malwert springt.
      minanz   : kleinster Wert, der bei der Ein-
                 gabe gemacht werden kann.

      Bsp.:

      dezval(3,4, 50, 80, 1);

      // Die Eingabe erfolgt an der Displaykoordinate
      // 3,4. Der Vorgabewert betraegt 50, Als
      // hoechste Eingabe kann 79 (value mod 80)
      // erfolgen, ein weiteres erhoehen setzt die
      // Eingabe auf 1
   -------------------------------------------------- */
uint8_t dezval_read(uint8_t x, uint8_t y, uint8_t startval, uint8_t divmod, uint8_t minanz)
{
  uint8_t acnt;

  gotoxy(x,y); put2dez(startval);
  while(!(is_buts() ))                          // wiederholen bis Set-Taste gedrueckt
  {
    if (is_butp() )                             // Plus-Taste ?
    {
      delay(50);                                // entprellen
      if (is_butp() )
      {
        acnt= 0;
        while (is_butp() && (acnt < autoenter)) // wird die Plus-Taste lange gedrueckt ?
        {
          acnt++;
          delay(autospeed);
        }
        if (acnt== autoenter)                   // dann schnelles hochzaehlen
        {
          while(is_butp())
          {
            startval= dezval_add(x,y, startval, divmod, minanz);
            delay(autospeed);
          }
        }
        else
        {
          startval= dezval_add(x,y, startval, divmod, minanz);
        }
      }
      else
      {
        startval= dezval_add(x,y, startval, divmod, minanz);
      }
    }

    if (is_butm() )                             // Minus-Taste ?
    {
      delay(50);                                // entprellen
      if (is_butm() )
      {
        acnt= 0;
        while (is_butm() && (acnt < autoenter)) // wird die Minus -Taste lange gedrueckt ?
        {
          acnt++;
          delay(autospeed);
        }
        if (acnt== autoenter)                   // dann schnelles runterzaehlen
        {
          while(is_butm())
          {
            startval= dezval_dec(x,y, startval, divmod, minanz);
            delay(autospeed);
          }
        }
        else
        {
          startval= dezval_dec(x,y, startval, divmod, minanz);
        }
      }
      else
      {
        startval= dezval_dec(x,y, startval, divmod, minanz);
      }
    }

  }
  delay(50);                                    // entprellen
  while(is_buts() );                            // warten bis Taste wieder losgelassen
  delay(50);
  return startval;
}


/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(void)
{
  struct my_datum date;
  uint8_t oldsek = 0;
  static const char tagnam[7][3] PROGMEM =
  {
    "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"
  };


  ssd1306_init();
  button_init();
  clrscr();

  while(1)
  {
    delay(100);
    date= rtc_readdate();
    if (oldsek != date.sek)
    {
      gotoxy(2,1);
      // Stunden und Minuten groesser anzeigen
      doublechar= 1;
      puthex(date.std); my_putchar(':');
      puthex(date.min);
      doublechar= 0;
      // Sekunden kleiner anzeigen
      puthex(date.sek);

      // Wochentag berechnen
      date.dow= rtc_getwtag(&date);

      // und Datum anzeigen
      gotoxy(2,5);
      oled_putromstring(&tagnam[date.dow][0]);
      my_putchar(' ');
      puthex(date.tag); my_putchar('.');
      puthex(date.monat); my_putchar('.');
      prints("20");
      puthex(date.jahr);
      oldsek= date.sek;
    }
    if (is_buts())                                // Uhr stellen, buts = "Uhr stellen - Taste"
    {
      clock_setscreen();
      delay(50);
      while(is_buts());
      delay(50);

      // Tag, Monat, Jahr einlesen
      date.tag= dezval_read(9,0, rtc_bcd2dez(date.tag), 32, 1);
      date.monat= dezval_read(9,1, rtc_bcd2dez(date.monat), 13, 1);
      date.jahr= dezval_read(9,2, rtc_bcd2dez(date.jahr), 100, 1);

      // Stunde, Minute, Sekunde einlesen
      date.std= dezval_read(9,4, rtc_bcd2dez(date.std), 24, 0);
      date.min= dezval_read(9,5, rtc_bcd2dez(date.min), 60, 0);
      date.sek= dezval_read(9,6, rtc_bcd2dez(date.sek), 60, 0);

      clrscr();
      // Eingaben ins BCD-Format umrechnen
      date.tag= dez2bcd(date.tag);
      date.monat= dez2bcd(date.monat);
      date.jahr= dez2bcd(date.jahr);
      date.std= dez2bcd(date.std);
      date.min= dez2bcd(date.min);
      date.sek= dez2bcd(date.sek);

      // in RTC-Chip speichern
      rtc_writedate(&date);
      clrscr();

    }
  }
}
