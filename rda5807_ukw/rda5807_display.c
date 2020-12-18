/* -------------------------------------------------------
                       rda5807_display.c

     Testprogramm fuer das UKW-Modul RDA5807M  mit
     I2C Interface. Bedienung ueber Tasten mit Ausgabe
     der eingestellten Empfangsfrequenz und aktuell
     eingestellten Lautstaerke auf I2C OLED-Display


     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz

     11.10.2018  R. Seelig

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
#include <avr/eeprom.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "rda5807.h"
#include "oled1306_i2c.h"


// rotate_oled definiert, ob ein Display auf dem Kopf stehend montiert wird. Ist
// dieses der Fall, muss zum Programm oled1306rot_i2c.c anstelle von oled1306_i2c.c
// hinzu gelinkt werden (Makefile entsprechend modifizieren)
#define rotate_oled      1                            // 0 : Ausgabe erfolgt "normal"
                                                      // 1 : Ausgabe erfolgt 180 Grad gedreht

#define buttonm_init()   PB2_input_init()
#define buttonp_init()   PA7_input_init()
#define buttons_init()   PA0_input_init()
#define button_init()    { buttonp_init(); buttonm_init(); buttons_init(); }

#define buttonm()        (~(is_PB2()) & 1)
#define buttonp()        (~(is_PA7()) & 1)
#define buttons()        (~(is_PA0()) & 1)

#define delay            _delay_ms

#define button_ptime     40     // Entprellzeit der Buttons
uint8_t funcsel =  0;           // Funktionsselekt der Buttons:
                                // 0: Sendersuchlauf
                                // 1: Sender manuell
                                // 2: Volume

uint8_t button_getchar(void)
{
  uint8_t keynr, retvalue;

  do                            // Tasten scannen bis eine Taste gedrueckt ist
  {
    keynr= (buttons()) | (buttonp()<<1) | (buttonm()<<2);
  } while(!(keynr));
  delay(button_ptime);          // Wartezeit zum Entprellen

  retvalue= ((buttons()) | (buttonp()<<1) | (buttonm()<<2)) + (funcsel * 0x10);     // Hi-Nibble beinhaltet Funktion

  //                            // Tasten scannen bis alle losgelassen
  do
  {
    keynr= (buttons()) | (buttonp()<<1) | (buttonm()<<2);
  } while (keynr);
  delay(button_ptime);          // Wartezeit zum Entprellen
  return retvalue;
}


/* ------------------------------------------------------------
     oled_putint

     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.

     Bsp.: 12345 wird als 123.45 ausgegeben.
     (ermoeglicht Pseudofloatausgaben im Bereich)
   ------------------------------------------------------------ */
void oled_putint(int i, char komma)
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

/* ----------------------------------------------------------
   oled_puts

   gibt einen Text an der aktuellen Position aus

      c : Zeiger auf den AsciiZ - String im RAM
   ---------------------------------------------------------- */
void oled_puts(char *c)
{
  while (*c)
  {
    oled_putchar(*c++);
  }
}

/* --------------------------------------------------
     tune_show

     zeigt die aktuell eingestellte Empfangsfrequenz
     an
   -------------------------------------------------- */
void tune_show(void)
{
  char i;

  doublechar= 1;
  gotoxy(0,0);
  if (aktfreq> 999)
  {
    oled_putint(aktfreq, 1);
  }
  else
  {
    oled_putchar(' ');
    oled_putint(aktfreq, 1);
  }
  doublechar= 0;
}

/* --------------------------------------------------
     volume_show

     zeigt die aktuell eingestellte Lautstaerke an
   -------------------------------------------------- */
void volume_show(void)
{
  char i;

  #if (rotate_oled == 1)
    // Klimmzuege, weil bei beschreiben des OLED-Ram der Adresspointer automatisch
    // inkrementiert wird, aber bei auf dem Kopf stehenden Display ein dekrementieren
    // notwendig waere. Aus diesem Grund wird der Lautstaerke Bargraph von hinten
    // nach vorne gezeichnet (im Gegensatz zu einem nicht auf dem Kopf stehenden
    // Display
    gotoxy(14,6);
    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< ((16-aktvol)*4); i++)
    {
      i2c_write(0x00);
    }
    for (i= 0; i< (aktvol*4); i++)
    {
      i2c_write(0xff);
    }
    i2c_stop();
  #else
    gotoxy(6,6);
    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< 60; i++)
    {
  //    i2c_write(0x24);
      i2c_write(0x00);
    }
    i2c_stop();

    gotoxy(6,6);
    i2c_start(ssd1306_addr);
    i2c_write(0x40);
    for (i= 0; i< (aktvol << 2); i++)
    {
      i2c_write(0xff);
    }
    i2c_stop();
  #endif
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
  tune_show();
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
  } while ((siglev < sigschwelle) && (aktfreq > fbandmin));

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
  } while ((siglev < sigschwelle) && (aktfreq < fbandmax));

  rda5807_setvol(aktvol);

}

/* --------------------------------------------------
                      mode_show
     zeigt aktuell eingestellten Modus an.

     Uebergabe:
        mode    : 0 fuer auto
                  1 fuer manual
                  2 fuer Volume
   -------------------------------------------------- */

void mode_show(uint8_t mode)
{
  gotoxy(6,3);
  switch (mode)
  {
   case 0 : oled_puts("auto  "); break;
   case 1 : oled_puts("manual"); break;
   case 2 : oled_puts("Volume"); break;
  }
}

/* --------------------------------------------------
                      screen_show

     zeigt Textmaske auf dem Display an.

     Uebergabe:
        mode    : 0 fuer auto
                  1 fuer manual
                  2 fuer Volume
   -------------------------------------------------- */
void screen_show(uint8_t mode)
{
  gotoxy(12,0); oled_puts("MHz");
  gotoxy(0,3); oled_puts("Mode: ");
  gotoxy(0,6); oled_puts("Vol: [        ]");
  mode_show(mode);
}


/* ---------------------------------------------------------------------
                                   MAIN
   --------------------------------------------------------------------- */
int main(void)
{
  char     ch, ch2, i;
  uint16_t storedchannel;
  uint8_t  keynr;

  delay(500);
  i2c_master_init();
  rda5807_init();
  ssd1306_init();
  button_init();
  delay(800);
  rda5807_init();                           // ein zweites Initialisieren, weil nach anlegen der
                                            // der Spannung manchmal der Chip nicht korrekt gestartet wurde.
                                            // Startet der Controller zu schnell und ist die Spannung noch nicht
                                            // stabil?

  clrscr();
  screen_show(0);

  storedchannel= eeprom_read_word((uint16_t*)0x04);    // gespeicherte Empfangsfrequenz aus EEPROM lesen
  if (storedchannel != 0xffff)              // und bei Gueltigkeit
  {
    setnewtune(storedchannel);              // diese einstellen
  }

  tune_show();
  volume_show();
  funcsel= 2;
  mode_show(funcsel);

  while(1)
  {
    ch= button_getchar();
    ch2= ch & 0x07;
    if (ch2 == 0x01)                        // Funktionsselekt
    {
      funcsel++;
      funcsel = funcsel % 3;                // Funktionen 0 .. 2
                                            // 0: Sendersuchlauf, 1: manuell, 2: Volume
      mode_show(funcsel);
    }
    switch (ch)
    {
      case 0x24 :                            // Volume erhoehen
      {
        if (aktvol< 15)
        {
          aktvol++;
          rda5807_setvol(aktvol);
          volume_show();
        }
        break;
      }

      case 0x22 :
      {
        if (aktvol> 0)                      // Volume verringern
        {
          aktvol--;
          rda5807_setvol(aktvol);
          volume_show();
        }
        break;
      }

      case 0x12 :                           // Empfangsfrequenz nach unten
      {
        if (aktfreq > fbandmin)
        {
          aktfreq--;
          setnewtune(aktfreq);
          eeprom_write_word((uint16_t*)0x04, aktfreq);
        tune_show();
        }
      break;
      }

      case 0x14 :                           // Empfangsfrequenz nach oben
      {
        if (aktfreq < fbandmax)
        {
          aktfreq++;
          setnewtune(aktfreq);
          eeprom_write_word((uint16_t*)0x04, aktfreq);
          tune_show();
        }
        break;
      }

      case 2 :                             // Suchlauf nach unten
      {
        rda5807_scandown();
        eeprom_write_word((uint16_t*)0x04, aktfreq);
        tune_show();

        break;
      }

      case 4 :                             // Suchlauf nach oben
      {
        rda5807_scanup();
        eeprom_write_word((uint16_t*)0x04, aktfreq);
        tune_show();

        break;
      }

      default  : break;
    }

  }
}
