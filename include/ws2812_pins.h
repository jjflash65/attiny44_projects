/* ----------------------------------------------------------
                          ws2812_pins.h

     Includedatei fuer ws2812.c und ws2812_output.S zum
     Festlegen des Portpins, an den eine WS2812 LED-Kette
     angeschlossen ist.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegung ATtiny44 siehe ws2812.h

     08.11.2018  R. Seelig
   ---------------------------------------------------------- */

#ifndef in_ws2812_pins
  #define in_ws2812_pins


  // Anschlusspin der WS2812 LED-Kette
  #define ws_port      PORTB
  #define ws_ddr       DDRB
  #define ws_portpin   1

#endif
