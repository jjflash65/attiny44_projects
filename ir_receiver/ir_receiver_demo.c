/* -------------------------------------------------------
                       ir_receiver_demo.c

     Versuche zu IR-Fernbedienungsempfaenger HX1838
     (38 kHz), Realisierung unter Verwendung Timer1

     Hardware : HX1838 IR-Empfaenger
                LED
     MCU      : ATtiny44
     Takt     : interner Takt 8 MHz

     Pinbelegung :

       LED         ---- PA4(9)
       Dout HX1839 ---- PB0(2)


     24.09.2018  R. Seelig
   ------------------------------------------------------ */

/*
         +5 VV             +5V        +5V                        +5V
           ^                ^          ^                          ^
           |                |          |                          |
           |                |          |                          |
           |                |          |                          |
           |                #          #                          |
           |            560 #      10k #                          |
           |                #          #                          |
           |                |          |                          |
           |                |          |       +-------------+    |
           |                |    ^^    |       |   HX1838    |    |
           |               ---  //     |       |             |    |
    -------------+         \ /         |       | OUT GND +5V |    |
                 |          v          |       +-------------+    |
         A       |         ---         |          |   |   |       |
         T       |          |  LED     |          |   |   |       |
         t       | 9        |          |          |   |   +-------+
         i   PA4 |----------+          |          |   |
         n       | 5                   |          |   |
         y   PB2 |---------------------+----------+   |
         4       |                                    |
         4       |                                    |
                 | 8                                  |
    PA5 DO (TxD) |-------------o zu RxD Computer      |
                 | 7                                  |
    PA6 DI (RxD) |-------------o zu TxD Computer      |
                 |                                    |
                 |                                    |
    -------------+                                   ---
           |
           |
           |
          ---
*/


#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_gpio.h"
#include "hx1838.h"
#include "usiuart.h"
#include "my_printf.h"

#define  printf            my_printf

#define led1_init()        PA4_output_init()
#define led1_set()         PA4_set()
#define led1_clr()         PA4_clr()

#define delay              _delay_ms

#define ir_keyblink        0x0cf3             // Coder der Fernbedienung mittels Terminal auslesen
                                              // und hier eintragen


/* --------------------------------------------------------
   my_putchar

   wird von my_printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   my_printf dann ein Zeichen ausgibt.
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  uart_putchar(ch);
}


/* ------------------------------------------------------------------------------
                                     M A I N
    ----------------------------------------------------------------------------- */
void main(void)
{
  uint8_t blinkenflag = 0;

  led1_init();
  uart_init();
  hx1838_init();

  printf("\n\n\rATtiny44: HX1838 IR-Receiver demo \n\n\r");

  led1_clr();                                 // Betriebsbereitschaft durch kurzes Blinken anzeigen
  delay(500);
  led1_set();

  while(1)
  {
    if (ir_newflag )                          // auf neu eingegangene Werte pollen
    {
      printf("\n\r IR-Receiver code: 0x%x ",ir_code);
      ir_newflag= 0;

      if (ir_code== ir_keyblink)              // Code auf der Beispielsfernbedienung fuer An / Aus
      {
        blinkenflag= blinkenflag ^ 0xff;
        if (blinkenflag) printf(" | blinken an  ... "); else printf(" | blinken aus ... ");
      }
    }

    if (blinkenflag)
    {
      led1_clr();
      delay(200);
      led1_set();
      delay(200);
    }
  }
}
