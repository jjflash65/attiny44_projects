/* ------------------------------------------------------------------
                             am433_rxdemo.c

     Demo fuer Datenuebertragung mittels einfachem AM 433 MHz
     Senderpaerchen.

                               Empfaenger

     Programm wartet so lange, bis auf der Funkstrecke ein Zeichen
     eingeht. Gehoert dieses zu einer der 3 Tasten  wird dieser
     die dieser Taste zugeordnete LED getoggelt (an/aus). Nach
     dem Togglen wird ein String eingelesen der vom Sender stammt
     anschliessend ein 16 Bit Integer.

     Der String und der Integer wird auf einem angeschlossenes I2C
     OLED Display ausgegeben.

     Der Code fuer fuer Taste 3 toggelt die LED an und gleich wieder
     aus, waehrend Taste 1 und 2 ueber den Sender geschaltet werden


     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz
     Fuses :  fuer 8 MHz intern
              lo 0xe2
              hi 0xdf


     22.11.2018  R. Seelig
   ------------------------------------------------------------------ */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "avr_gpio.h"
#include "am433_transceiv.h"

#define delay     _delay_ms

#define ledr_init()      PA2_output_init()
#define ledr_set()       PA2_set()
#define ledr_clr()       PA2_clr()
#define red              0

#define ledge_init()     PA3_output_init()
#define ledge_set()      PA3_set()
#define ledge_clr()      PA3_clr()
#define yellow           1

#define ledgn_init()     PA4_output_init()
#define ledgn_set()      PA4_set()
#define ledgn_clr()      PA4_clr()
#define green            2

#define led_init()      { ledr_init(); ledr_clr();        \
                          ledge_init(); ledge_clr();      \
                          ledgn_init(); ledgn_clr(); }


uint8_t toggleflag = 0;


/* --------------------------------------------------
                      led_toggle
   -------------------------------------------------- */
void led_toggle(uint8_t pos)
{

  toggleflag = toggleflag ^ (1 << pos);

  switch (pos)
  {
    case 0 :
    {
      if (toggleflag & 1) ledr_set(); else ledr_clr();
      break;
    }
    case 1 :
    {
      if (toggleflag & 2) ledge_set(); else ledge_clr();
      break;
    }
    case 2 :
    {
      if (toggleflag & 4) ledgn_set(); else ledgn_clr();
      break;
    }

    default:  break;
  }
}


/* --------------------------------------------------------------
                                 MAIN
   -------------------------------------------------------------- */
int main(void)
{
  uint8_t  errsum;
  uint8_t  rcvbyte;
  uint16_t rcvword;

  uint8_t  strbuf[36];

  led_init();
  am433_rxd_init();


  while(1)
  {
    rcvbyte= am433_receive(&errsum);                    // warte auf ein gesendetes Zeichen und liest dieses
    switch (rcvbyte)
    {
      case 1 : led_toggle(red); break;
      case 2 : led_toggle(yellow); break;
      case 3 : led_toggle(green); break;
      default  : break;
    }
  }
}
