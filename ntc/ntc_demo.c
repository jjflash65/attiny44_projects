/* ------------------------------------------------------------------
                              ntc_demo.c

     Temperatur ueber einen Spannungsteiler ermitteln. Pullup -
     Widerstand gegen Vcc, NTC gegen Masse verschaltet. Der
     Spannungsteilerabgriff liegt auf PA3 und der ADC misst somit
     die Spannung am NTC.

     Die Auswertung des NTC erfolgt Tabellengestuetzt. Die Tabelle
     kann mit dem Tabellengenerator im Verzeichnis ./generator
     erzeugt werden


     Hardware : PullUp Widerstand
                OLED I2C Display
                NTC Widerstand

     MCU      : ATtiny44
     F_CPU    : 8 MHz intern

     Fuses    : fuer 8 MHz intern
                lo 0xe2
                hi 0xdf

     Fuses    : fuer ext. Quarz >= 8 MHz
                lo 0xde
                hi 0xdf


     Pinbelegung I2C Display
     -----------------------

     PB0 = SDA
     PB1 = SCL

     Pinbelegung ADC
     ---------------

     PA3 = analoger Eingang


     Program:    3130 bytes (76.4% Full)
     (.text + .data + .bootloader)
     Data:         14 bytes (5.5% Full)
     (.data + .bss + .noinit)

     20.09.2018 R. Seelig
   ------------------------------------------------------------------ */

/*
                                                 ATtiny44
                                        4 kByte Flash / 256 Byte SRAM

                                             Anschlusspins IC

                                              +-----------+
                                         Vcc  | 1      14 |  GND
                   PCINT8 - XTAL1 - CLKI PB0  | 2   A  13 |  PA0 - ADC0 - AREF - PCINT0
                        PCINT9 - XTAL2 - PB1  | 3   T  12 |  PA1 - ADC1 - AIN0 - PCINT1
                 PCINT11 - /reset - dW - PB3  | 4   t  11 |  PA2 - ADC2 - AIN1 - PCINT2
         PCINT10 - INT0 - OC0A - CKOUT - PB2  | 5   i  10 |  PA3 - ADC3 - T0 - PCINT3
            PCINT7 - ICP - OC0B - ADC7 - PA7  | 6   n   9 |  PA4 - ADC4 - USCK - SCL - T1 - PCINT4
PCINT6 - OC1A - SDA - MOSI - DI - ADC6 - PA6  | 7   y   8 |  PA5 - ADC5 - DO - MISO - OC1B PCINT5
                                              +-----------+

*/

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "my_printf.h"
#include "oled1306_i2c.h"
#include "adc_single.h"

#define  delay                _delay_ms
#define  printf               my_printf

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
                          M-A-I-N
   ------------------------------------------------------- */
int main(void)
{
  uint16_t adc_value;

  printfkomma= 1;

  ssd1306_init();
  clrscr();

  adc_init(0, 3);                          // Analogeingang auf PA3, Vcc als Referenz

  gotoxy(2,0);
  printf("Tiny44 - ADC");
  doublechar= 1;
  while(1)
  {
    gotoxy(0,3);
    printf("%k %cC ", ntc_gettemp(adc_getvalue()), 0x81 );
    delay(400);
  }
}
