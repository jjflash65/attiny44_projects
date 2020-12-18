/* ----------------------- usi_serial_adc.c -------------------------

     Demoprogramm zur Verwendung des USI-Interfaces als
     UART - RS232 Schnittstelle

     Verwendet USI-Overflow, Pinchange0 und Timer0 Compare Match
     Interrupt


     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Quellen: AVR App-Note AVR307 und diverse Internetquellen

     TxD Anschluss: PA5 (DO)
     RxD Anschluss: PA6 (DI)

     17.08.2017 R. Seelig
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
#include <avr/io.h>

#include "usiuart.h"
#include "my_printf.h"
#include "avr_gpio.h"
#include "adc_single.h"

#define  printf        tiny_printf
#define  delay         _delay_ms

#define  intervall     400

#define  led_init()     PA1_output_init()
#define  led_clr()      PA1_set()
#define  led_set()      PA1_clr()

/* -------------------------------------------------------
                      u_in und Makro bin2spg

    u_in:
    Spannung in 0,1 mV, die vor einem (eventuellen)
    Spannungsteiler am ADC den 10 Bit Maximalwert (1023)
    erzeugt

    bin2spg:
    rechnet den Wert von u_in bei einer Aufloesung von
    10 Bit in einen Festkommazahlen - Spannungswert um.
    Wert von bspw. 382 entspricht hier dann 3.82 Volt

    Hinweis: Nach dieser Methode koennen bei Messbereich
    5V max. 2% Fehler durch Rundung entstehen
   ------------------------------------------------------- */
#define  u_in                 52000
#define  bin2spg(value)       ( value * ( u_in / 1023 ) / 100 )


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  uart_putchar(ch);
}


int main(void)
{

  uint8_t ch;

  printfkomma= 2;

  uart_init();
  led_init();
  adc_init(0, 3);                                                          // Betriebsspannung als Referenz,
                                                                           // Analogeingang auf PA3


  printf("\n\r--------------------------------------");
  printf("\n\rAttiny44 running at 8 MHz\n\r");
  printf("8 kByte Flash;  1 KByte RAM\n\n\r");
  printf("Baudrate: 19200 bd\n\r");
  printf("using: usiuart, adc, my_printf\n\n\r");
  printf("14.09.2018  R. Seelig\n\r");
  printf("--------------------------------------\n\n\r");

  while(1)
  {
    printf("                     \r");
    delay(1);
//    printf(" U in (PA3): %d V\r", adc_getvalue() );                      // zu Testzwecken
    printf(" U in (PA3): %k V\r", bin2spg(adc_getvalue()) );
    led_set();
    delay(intervall);
    led_clr();
  }
}
