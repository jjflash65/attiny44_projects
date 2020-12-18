/* -------------------------------------------------------
                        lcd_7segtherm.c

     Anbindung eines zweistelligen 7-Segment LCD's OHNE
     eigenen Controller ("pures Display") ueber
     2 kaskadierteSchieberegister SN74HC595 am Beispiel
     eines NTC-Thermometers.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegung siehe lcd_7segdemo.h

     05.11.2018  R. Seelig
   ------------------------------------------------------ */


#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "lcd_7seg.h"

#define  delay     _delay_ms

/* -------------------------------------------------
     Lookup-table fuer NTC-Widerstand
     R25-Wert: 10.00 kOhm
     Pullup-Widerstand: 4.70 kOhm
     Materialkonstante beta: 3950
     Einheit eines Tabellenwertes: 0.1 Grad Celcius
     Temperaturfehler der Tabelle: 0.1 Grad Celcius
   -------------------------------------------------*/
const int PROGMEM ntctable[] = {
  1953, 1629, 1305, 1132, 1013, 924, 851, 789,
  735, 687, 643, 603, 565, 529, 495, 462,
  430, 399, 368, 337, 306, 275, 242, 209,
  175, 138, 98, 54, 4, -56, -133, -251,
  -369
};

int ntc_gettemp(uint16_t adc_value)
{
  int p1,p2;

  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.
  p1 = pgm_read_word(&(ntctable[ (adc_value >> 5)    ]));
  p2 = pgm_read_word(&(ntctable[ (adc_value >> 5) + 1]));

  // zwischen beiden Punkten interpolieren.
  return p1 - ( (p1-p2) * (adc_value & 0x001f) ) / 32;
}


/* ---------------------------------------------------------------------------------
                                      M-A-I-N
   ---------------------------------------------------------------------------------*/
int main(void)
{
  uint16_t mw;

  lcd7s_init();
  adc_init(0, 3);                          // Analogeingang auf PA3, Vcc als Referenz

  while(1)
  {
    delay(250);
    lcd7s_dezout(ntc_gettemp(adc_getvalue())/10 );
  }
}
