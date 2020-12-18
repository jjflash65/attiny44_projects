/* -------------------------------------------------------
                        seg7_hc595_adc.c

     Testprogramm fuer 4 stelliges 7-Segmentmodul (China)
     mit 74HC595 Schieberegistern

     Zeigt den Spannungswert an PA3 auf dem Modul an

     Hardware : Chinamodul "4-Bit LED Digital Tube Modul"

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     17.09.2018  R. Seelig
   ------------------------------------------------------ */

/*
   Anschluesse:
 ------------------------------------------------------
   Pinbelegung:

   4 Bit LED Digital Tube Module                 ATtiny44
   -------------------------------------------------------

       (+Ub)           Vcc
       (shift-clock)   Sclk   -------------------- PA4 (9)
       (strobe-clock)  Rclk   -------------------- PA0 (13)
       (ser. data in)  Dio    -------------------- PA5 (8)
                       Gnd


   Anzeigenposition 0 ist das rechte Segment des Moduls

            +-----------------------------+
            |  POS3   POS2   POS1   POS0  |
    Vcc  o--|   --     --     --     --   |
    Sclk o--|  |  |   |  |   |  |   |  |  |
    Rclk o--|  |  |   |  |   |  |   |  |  |
    Dio  o--|   -- o   -- o   -- o   -- o |
    GND  o--|                             |
            |   4-Bit LED Digital Tube    |
            +-----------------------------+

*/

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "seg7_hc595.h"
#include "adc_single.h"

#define delay       _delay_ms

/* -------------------------------------------------------
                      u_in und Makro bin2spg

    u_in:
    Spannung in 0,1 mV, repraesentiert den Spannungswert
    der dem 10-Bit Maximalwert (1023) entspricht.

    bin2spg:
    rechnet den Wert von u_in bei einer Aufloesung von
    10 Bit in einen Festkommazahlen - Spannungswert um.
    Wert von bspw. 382 entspricht hier dann 3.82 Volt

    Hinweis: Nach dieser Methode koennen bei Messbereich
    5V max. 2% Fehler durch Rundung entstehen
   ------------------------------------------------------- */
#define  u_in                 52000              // 5,2V entsprechen Digitalwert 1023 (0x400)
#define  bin2spg(value)       ( value * ( u_in / 1023 ) / 100 )


/* ---------------------------------------------------------------------------------
                                      M-A-I-N
   ---------------------------------------------------------------------------------*/

int main(void)
{
  uint16_t   oldsek  = 1;
  uint16_t   counter = 0;

  digit4_init();                        // Modul initialisieren
  adc_init(2, 3);                       // Analogeingang auf PA3


  digit4_setdez(0000);

  digit4_setdp(2);                      // Kommapunkt anzeigen

  // Endlosschleife
  while(1)
  {
    delay(500);
    digit4_setdez(bin2spg(adc_getvalue()) );
  }
}
