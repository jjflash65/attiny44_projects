/* ----------------------------------------------------------
                          ws2812.h

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     Anschlussbelegung siehe ws2812.h

     08.11.2018  R. Seelig
   ---------------------------------------------------------- */

/*
                                                 ATtiny44
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

    Anschluss des Datenpins der LED-Kette in ws2812_pin.h
*/

#ifndef in_ws2812
  #define in_ws2812

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <string.h>

  #include "ws2812_pins.h"

  // Farbstruktur rot, gruen, blau
  typedef struct colvalue
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  } colvalue;

  #define ws_black      0
  #define ws_bluedrk    1
  #define ws_greendrk   2
  #define ws_cyandrk    3
  #define ws_reddrk     4
  #define ws_purpledrk  5
  #define ws_brown      6
  #define ws_grey       7
  #define ws_greydrk    8
  #define ws_blue       9
  #define ws_green      10
  #define ws_cyan       11
  #define ws_red        12
  #define ws_purple     13
  #define ws_yellow     14
  #define ws_white      15

  extern  const uint8_t PROGMEM egapalette[];

  /* ----------------------------------------------------------
                            Prototypen
     ---------------------------------------------------------- */
  void rgbfromega(uint8_t eganr, struct colvalue *f);
  void rgbfromvalue(uint8_t r, uint8_t g, uint8_t b, struct colvalue *f);
  void ws_showbuffer(uint8_t *ptr, uint16_t count);
  void ws_setrgbcol(uint8_t *ptr, uint16_t nr, struct colvalue *f);
  void ws_reset(void);
  void ws_init(void);
  void ws_clrarray(uint8_t *ptr, int anz);
  void ws_blendup_left(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime);
  void ws_blendup_right(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime);
  void ws_buffer_rl(uint8_t *ptr, uint8_t lanz);
  void ws_buffer_rr(uint8_t *ptr, uint8_t lanz);

#endif
