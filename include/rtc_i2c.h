/* ----------------------------------------------------------
                           rtc_i2c.c

     Header fuer Softwaremodul zu I2C Realtime clock Bau-
     steinen DS1307 und DS3231 (gleiche Steuercodes).

     Ansprechen des Bausteins erfolgt ueber Software I2C


     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL

     02.01.2019

   ---------------------------------------------------------- */


#ifndef in_rtc_i2c
  #define in_rtc_i2c

  #include <util/delay.h>
  #include <avr/io.h>
  #include "i2c_sw.h"

  #define rtc_addr            0xd0              // 8-Bit I2C Adresse: R/W Flag ist Bestandteil der Adresse !

  struct my_datum                               // Datum- und Uhrzeitsstruktur
  {
    uint8_t jahr;
    uint8_t monat;
    uint8_t tag;
    uint8_t dow;
    uint8_t std;
    uint8_t min;
    uint8_t sek;
  };


  // ----------------------------------------------------------
  //                       Prototypen
  // ----------------------------------------------------------

  uint8_t rtc_read(uint8_t addr);
  void rtc_write(uint8_t addr, uint8_t value);
  uint8_t rtc_bcd2dez(uint8_t value);
  uint8_t rtc_getwtag(struct my_datum *date);
  struct my_datum rtc_readdate(void);
  void rtc_writedate(struct my_datum *date);

#endif
