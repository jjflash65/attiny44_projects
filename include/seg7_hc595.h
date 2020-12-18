/* -------------------------------------------------------
                        seg7_hc595.h

     Headerdatei fuer 4 stelliges 7-Segmentmodul (China)
     mit 74HC595 Schieberegistern

     Anmerkung: leider muessen die Anzeigesegmente des
                Moduls gemultiplext werden, da nur
                2 Schieberegister enthalten sind.
                SR sind kaskadiert, zuerst ist der
                Datenwert der Ziffer, danach die
                Multiplexstelle hinauszuschieben.

     Hardware : Chinamodul "4-Bit LED Digital Tube Modul"

     Bemerkung: Damit die Anzeige gemultiplext werden
                kann, wird Timer1 als interruptbe-
                triebener Taktgeber eingesetzt.

                Dieser Interrupt zaehlt zusaetzlich die
                globale Variable millis hoch

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

#ifndef in_seg7anz_v2
  #define in_seg7anz_v2

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include "avr_gpio.h"

  //  Anschlusspins des Moduls
  #define srdata_init()       PA5_output_init()
  #define srdata_set()        PA5_set()
  #define srdata_clr()        PA5_clr()

  #define srstrobe_init()     PA0_output_init()
  #define srstrobe_set()      PA0_set()
  #define srstrobe_clr()      PA0_clr()

  #define srclock_init()      PA4_output_init()
  #define srclock_set()       PA4_set()
  #define srclock_clr()       PA4_clr()

  extern uint8_t  seg7_4digit[4];
  extern uint8_t  led7sbmp[16];

  extern volatile uint32_t  millis;
  extern volatile uint32_t  volatile tim1_sek;


  /* ----------------------------------------------------------
     Prototypen
     ---------------------------------------------------------- */

  void digit4_delay(void);
  void digit4_ckpuls(void);
  void digit4_stpuls(void);
  void digit4_outbyte(uint8_t value);
  void digit4_setdez(int value);
  void digit4_setdez8bit(uint8_t value, uint8_t pos);
  void digit4_sethex(uint16_t value);
  void digit4_setall(uint8_t c3, uint8_t c2, uint8_t c1, uint8_t c0);
  void digit4_setdp(char pos);
  void digit4_clrdp(char pos);
  void digit4_init(void);

  void timer1_init(void);           // fuer den Multiplexbetrieb


#endif
