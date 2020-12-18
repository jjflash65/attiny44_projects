/* -----------------------------------------------------
                        rda5807.h

    Header fuer Softwaremodul RDA5807 UKW- Empfaengers
    (I2C Interface)

     MCU      : ATtiny44
     F_CPU    : 8 MHz intern

     Hardware : RDA5807 IC

     Fuses    : fuer 8 MHz intern
                 lo 0xe2
                 hi 0xdf

     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL


     28.09.2018 R. Seelig

  ------------------------------------------------------ */

#ifndef in_rda5807
  #define in_rda5807

  #include <util/delay.h>
  #include <avr/io.h>

  #include "avr_gpio.h"
  #include "i2c_sw.h"

  #define fbandmin     870        // 87.0  MHz unteres Frequenzende
  #define fbandmax     1080       // 108.0 MHz oberes Frequenzende
  #define sigschwelle  84         // Schwelle ab der ein Sender als "gut empfangen" gilt

  #define delay        _delay_ms

  extern uint16_t aktfreq;        // Startfrequenz ( 101.8 MHz )
  extern uint8_t  aktvol;         // Startlautstaerke
  extern uint16_t festfreq[6];

/* -----------------------------------------------------
                         PROTOTYPEN
   ----------------------------------------------------- */

  void rda5807_writereg(char reg);
  void rda5807_write(void);
  void rda5807_reset(void);
  void rda5807_poweron(void);
  int rda5807_setfreq(uint16_t channel);
  void rda5807_setvol(int setvol);
  void rda5807_setstereo(void);
  uint8_t rda5807_getsig(void);
  void rda5807_scandown(void);

#endif
