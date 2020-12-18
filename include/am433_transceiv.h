/* ------------------------------------------------------------------
                             am433_transceiv.h

     Header Softwaremodul fuer Datenuebertragung mit sehr billigen
     AM 433 MHz Modulen (Sender / Empfaenger: Startstop-Oszillator
     und Pendelaudion)

     Die Software verwendet zur Uebertragung einfachstes Bitbanging
     nach RS232 Protokoll. Einzelne Bytes schicken ueber dieses
     Protokoll zuerst ein Zerobyte damit der Oszillator in Betrieb
     ist, danach ein Synchronisierungsbyte
                                0xaa
     welches kennzeichnet, dass, je nachdem ob ein String, ein
     Byte oder ein Integer uebertragen wird, alle nachfolgende
     Bytes Datenbyte(s) sind. Im Anschluss daran wird ein Check-
     summenbyte gesendet

     MCU   :  ATtiny44
     Takt  :  interner Takt 8 MHz
     Fuses :  fuer 8 MHz intern
              lo 0xe2
              hi 0xdf


     22.11.2018  R. Seelig
   ------------------------------------------------------------------ */

#ifndef in_AM433
  #define in_AM433

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>

  #include "avr_gpio.h"

  #define delay     _delay_ms


  // Portpinanschluss, auf dem gesendet wird
  #define txd_set()           PB1_set()
  #define txd_clr()           PB1_clr()
  #define am433_txd_init()    { PB1_output_init(); txd_clr(); }

  // Portpinanschluss, auf dem empfangen wird
  #define rxd_isbit()         is_PA0()
  #define am433_rxd_init()    PA0_input_init()


  #define baudrate     1200                         // Baudrate RS232 Protokoll
  #define bauddelay    ((1000000 / baudrate))       // Verzoegerungszeit entsprechend Baudrate
  #define hbauddelay   (bauddelay / 2)              // halbe Baudratenzeit

  #define delafterz    12                           // Verzoegerung in ms nach initialem Zerobyte

/* --------------------------------------------------
                      Prototypen
   -------------------------------------------------- */

  // Bitbanging RS232
  void swuart_putchar(uint8_t value);
  uint8_t swuart_getchar();

  // Funkstrecke
  void am433_transmit(uint8_t value);
  void am433_transmitword(uint16_t value);
  void am433_transmitstr(uint8_t *string);
  void am433_transmitstr2(uint8_t *string);
  uint8_t am433_receive(uint8_t *errsum);
  uint16_t am433_receiveword(uint8_t *errsum);
  void am433_receivestr(uint8_t *string);
  void am433_receivestr2(uint8_t *string);


#endif
