/* ------------------------------------------------------------------
                             am433_txdemo.c

     Demo fuer Datenuebertragung mittels einfachem AM 433 MHz
     Senderpaerchen.

                                Sender

           Mittels des Senders wird der Code einer der 3
           Tasten gesendet, wenn diese gedrueckt und wieder
           losgelassen wird.


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

#include "avr_gpio.h"
#include "am433_transceiv.h"

#define delay     _delay_ms


// Pinports der angeschlossenen Tasten
#define button1_init()      PA1_input_init()
#define is_button1()        (!(is_PA1()))

#define button2_init()      PA2_input_init()
#define is_button2()        (!(is_PA2()))

#define button3_init()      PA3_input_init()
#define is_button3()        (!(is_PA3()))




/* --------------------------------------------------
                        button_get

     liefert den Code der Taste (0..2) zurueck, die
     gedrueckt und wieder losgelassen wurde
   -------------------------------------------------- */
uint8_t button_get(void)
{
  if (is_button1())
  {
    delay(30);
    while(is_button1() );       // warten bis Taste wieder losgelassen wurde
    delay(30);
    return 1;
  }
  if (is_button2())
  {
    delay(30);
    while(is_button2() );       // warten bis Taste wieder losgelassen wurde
    delay(30);
    return 2;
  }
  if (is_button3())
  {
    delay(30);
    while(is_button3() );       // warten bis Taste wieder losgelassen wurde
    delay(30);
    return 3;
  }
  return 0;
}


/* ------------------------------------------------------------------
   ------------------------------------------------------------------ */
int main(void)
{
  uint8_t ch;
  uint16_t cnt;

  uint8_t msg1[] = "Hallo Welt    ";
  uint8_t msg2[] = "433MHz wirel. ";
  uint8_t msg3[] = "              ";

  button1_init();
  button2_init();
  button3_init();

  am433_txd_init();
  cnt= 0;

  while(1)
  {
    ch= button_get();
    if (ch)
    {
      cnt++;
      am433_transmit(ch);
/*
      switch(ch)
      {
        case 1 : am433_transmitstr(&msg1[0]); am433_transmitword(cnt); break;
        case 2 : am433_transmitstr(&msg2[0]); am433_transmitword(cnt); break;
        case 3 : am433_transmitstr(&msg3[0]); am433_transmitword(cnt); break;
      }
*/
    }
    delay(100);
  }
}
