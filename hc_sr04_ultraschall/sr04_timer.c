/* ------------------------------------------------------------------
                              sr04_timer.c

     Funktionstest fuer HC-SR04 Entfernungssensor mittels Ultraschall-
     echos.


     MCU      : ATtiny44
     F_CPU    : 8 MHz intern

     Hardware : Sensor HC-SR04
                Display OLED mit SSD1306 Controller
                Lautsprecher

     Fuses    : fuer 8 MHz intern
                lo 0xe2
                hi 0xdf

     Fuses    : fuer ext. Quarz >= 8 MHz
                lo 0xde
                hi 0xdf


     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL

     Pinbelegung HC-SR04
     -------------------

     PA7 = trigger
     PA6 = echo

     Pinbelegung Lautsprecher
     ------------------------

     PA1 = Lautsprecher


     Hinweis:  Programm benoetigt Timer0 (im Interrupt fuer
               Lautsprecher) und Timer1 zum Zaehlen (ohne Interrupt)

     18.10.2018 R. Seelig
   ------------------------------------------------------------------ */


#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "oled1306_i2c.h"
#include "toene.h"

#define  delay                _delay_ms
#define  prints               putstring


// Kalibrierfaktor experimentell ermittelt. Groesserer Zahlenwert ergibt
// kleinere Absolutentfernung per Zaehlimpuls (abhaengig von Taktfrequenz)
#define scalefactor   74                   // Faktor bei 8 MHz


// ----------  Ultraschallsensor: Anbindung  -------------

#define sr04_init()    { PA7_output_init(); PA6_input_init(); }

#define sr04_isecho()  is_PA6()
#define sr04_trighi()  PA7_set()
#define sr04_triglo()  PA7_clr()


// Bezeichner Taktvorteiler Timer/Counter 0 (16 Bit)
enum { pscale1 = 1, pscale8, pscale64, pscale256, pscale1024 };


/* ------------------------------------------------------------
                           Helpers
   ------------------------------------------------------------ */

/* ------------------------------------------------------------
                           mydelay

     unkalibrierte Verzoegerungsschleife fuer Mikrosekunden
   ------------------------------------------------------------ */
void mydelay(int value)
{
  uint16_t speed;

  for (speed= 0; speed< value; speed++) _delay_us(10);
}


/* ------------------------------------------------------------
                            putint
     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.

     Bsp.: 12345 wird als 123.45 ausgegeben.
     (ermoeglicht Pseudofloatausgaben im Bereich)
   ------------------------------------------------------------ */
void putint(int i, char komma)
{
  typedef enum boolean { FALSE, TRUE }bool_t;

  static int zz[]      = { 10000, 1000, 100, 10 };
  bool_t     not_first = FALSE;

  uint8_t       zi;
  int        z, b;

  komma= 5-komma;

  if (!i)
  {
    oled_putchar('0');
  }
  else
  {
    if(i < 0)
    {
      oled_putchar('-');
      i = -i;
    }
    for(zi = 0; zi < 4; zi++)
    {
      z = 0;
      b = 0;

      if  ((zi==komma) && komma)
      {
        if (!not_first) oled_putchar('0');
        oled_putchar('.');
        not_first= TRUE;
      }

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }
      if(b || not_first)
      {
        oled_putchar('0' + b);
        not_first = TRUE;
      }
      i -= z;
    }
    if (komma== 4) oled_putchar('.');
    oled_putchar('0' + i);
  }
}

/* --------------------------------------------------
                      putstring
     gibt einen String aus dem RAM aus
   -------------------------------------------------- */
void putstring(uint8_t *p)
{
  do
  {
    oled_putchar( *p );
  } while( *p++);
}


/* --------------------------------------------------
                          MAIN
   -------------------------------------------------- */
int main(void)
{
  uint16_t   cnt;

  ssd1306_init();
  clrscr();

  toene_init();
  settonfreq(500);

  sr04_init();
  sr04_trighi();

  gotoxy(0,0);
  prints(" Ultraschall\n\r SR04-Sensor");
  TCCR1B = pscale64;            // Taktvorteiler Counter1 = F_CPU / 64

  while(1)
  {
    // Startimpuls

    sr04_triglo();
    _delay_us(10);
    sr04_trighi();

    cli();                                           // Interrupts zum zaehlen sperren (Toeneinterrupt ist aktiv)
    while( (!sr04_isecho() ));                       // warten, bis der Sensor mit logisch 1 antwortet.

    TCNT1= 0;                                        // Counter = 0
    while( (sr04_isecho() || (cnt== 0xffff)) )       // zaehlen, bis Echo ankommt oder Ueberlauf erfolgt ist
    {
      cnt= TCNT1;
    }
    sei();

    cnt= (cnt*10) / scalefactor;                     // gezaehlte Ticks in Zentimeter umrechnen

    gotoxy(1,3);
    putint(cnt,0);
    prints(" cm       ");
    gotoxy(1,5);

    if (cnt < 8)
    {
      prints("zu nah");
      sound= 1;
      mydelay(1000*cnt);
      sound= 0;
      mydelay(1000*cnt);

    }
    else
    {
      prints("o.k.  ");
      _delay_ms(200);
    }

  }
}
