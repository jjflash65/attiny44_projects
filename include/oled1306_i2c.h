/* -----------------------------------------------------
                        oled1306_i2c.h

    Header fuer das Anbinden eines OLED Displays mit
    SSD1306 Controller und I2C Interface

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


     12.09.2018 R. Seelig

  ------------------------------------------------------ */

#ifndef in_i2c_devices
  #define in_i2c_devices

  #include <util/delay.h>
  #include "avr_gpio.h"
  #include "i2c_sw.h"
  #include <avr/pgmspace.h>



  #define ssd1306_addr          0x78

  extern uint8_t aktxp;
  extern uint8_t aktyp;
  extern uint8_t doublechar;
  extern uint8_t bkcolor;
  extern uint8_t textcolor;

  void ssd1306_init(void);
  void gotoxy(uint8_t x, uint8_t y);
  void clrscr(void);
  void oled_putchar(uint8_t ch);


#endif
