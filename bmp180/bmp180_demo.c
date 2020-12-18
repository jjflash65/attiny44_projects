/* ----------------------------------------------------------
     bmp180_demo.c

     MCU     : attiny44
     F_CPU   : 8000000

     19.12.2018

   ---------------------------------------------------------- */

#include <util/delay.h>
#include <avr/io.h>
#include <stdbool.h>
#include <string.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "my_printf.h"
#include "i2cmaster.h"
#include "bmp085.h"

#define printf   my_printf
#define delay    _delay_ms

void my_putchar(char ch)
{
  uart_putchar(ch);
}


/* ---------------------------------------------------------------------------
                                    M A I N
   --------------------------------------------------------------------------- */
int main(void)
{
  uart_init();

  uint8_t ch;
  uint8_t cx,anz;
  uint8_t ack;


  printfkomma= 3;

  uart_init();
  i2c_init();

  while(1)
  {
    printf("\n\rI2C Bus scanning\n\r--------------------------\n\n\r" \
           "Devices found at address:\n\n\r");

    anz= 0;

    for (cx= 0; cx< 254; cx += 2)
    {
      ack= (~i2c_start(cx)) & 1;
//      delay(1);
      i2c_stop();
      if (ack)
      {
        switch (cx)
        {
          case 0xC0 : printf("Adr. %xh : TEA5767 UKW-Radio\n\r", cx); break;
          case 0x20 :
          case 0x22 : printf("Adr. %xh : RDA5807 UKW-Radio\n\r", cx); break;
          case 0x40 :
          case 0x42 :
          case 0x44 :
          case 0x46 :
          case 0x48 :
          case 0x4A :
          case 0x4C :
          case 0x4E : printf("Adr. %xh : PCF8574 I/O Expander\n\r", cx); break;
          case 0x90 :
          case 0x92 :
          case 0x94 :
          case 0x96 :
          case 0x98 :
          case 0x9A :
          case 0x9C :
          case 0x9E : printf("Adr. %xh : LM75 Temp.-Sensor\n\r", cx); break;
          case 0xA0 :
          case 0xA2 :
          case 0xA4 :
          case 0xA6 :
          case 0xA8 :
          case 0xAA :
          case 0xAC :
          case 0xAE : printf("Adr. %xh : EEProm\n\r", cx); break;
          case 0x78 : printf("Adr. %xh : SSD13016 I2C-OLED Display\n\r", cx); break;
          case 0xD0 : printf("Adr. %xh : RTC - DS1307\n\r", cx); break;

          default   : printf("Adr. %xh : unknown\n\r",cx); break;
        }
      }
      delay(1);
    }
    printf("\n\n\rEnd of I2C-bus scanning... \n\n\r");
    i2c_stop();
    printf("Press any key for rescan... \n\r");
    ch= uart_getchar();
//    printf("Taste war: %c\n\n\r", ch);
  }
  while(1);
}
