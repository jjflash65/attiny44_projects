/* -------------------------------------------------------
                        seg7_hc595_demo.c

     Testprogramm fuer 4 stelliges 7-Segmentmodul (China)
     mit 74HC595 Schieberegistern

     Anmerkung: leider muessen die Anzeigesegmente des
                Moduls gemultiplext werden, da nur
                2 Schieberegister enthalten sind.
                SR sind kaskadiert, zuerst ist der
                Datenwert der Ziffer, danach die
                Multiplexstelle hinauszuschieben.

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

   Segmentbelegung der Anzeige:

       a
      ---
   f | g | b            Segment | dp |  g  |  f  |  e  |  d  |  c  |  b  |  a  |
      ---               --------------------------------------------------------
   e |   | c            Bit-Nr. |  7 |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
      ---
       d

   Segmente leuchten bei einer logischen 0 (gemeinsame Kathode) !!!

*/

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "seg7_hc595.h"

#define d4char_C    0xc6       // Bitmuster fuer grosses C
#define d4char_oo   0x9c       // Bitmuster fuer hochgestelltes o

#define delay       _delay_ms

/* ---------------------------------------------------------------------------------
                                      M-A-I-N
   ---------------------------------------------------------------------------------*/

int main(void)
{
  uint16_t   oldsek  = 1;
  uint16_t   counter = 0;

  digit4_init();                        // Modul initialisieren

  digit4_setdez(1234);
  delay(1500);

 // Bsp. Anzeige von 15 Grad Celcius
  digit4_setdez8bit(15,2);
  seg7_4digit[1]= d4char_oo;
  seg7_4digit[0]= d4char_C;

  delay(1500);

  digit4_setdp(2);                      // Kommapunkt anzeigen

  // endlose Zaehlschleife
  while(1)
  {
    if (oldsek != tim1_sek)
    {
      counter++;
      counter= counter % 10000;
      digit4_setdez(counter);
      oldsek= tim1_sek;
    }
  }
}
