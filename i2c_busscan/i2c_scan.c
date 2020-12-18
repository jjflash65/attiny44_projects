/* ------------------------------------------------------------------
                              i2c_scan.c

     Software I2C Bitbanging mit ATtiny44

     Scant den I2C Bus nach angeschlossenen Teilnehmern und gibt
     die Hexadresse des I2C-Device auf dem UART aus.


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


     Pinbelegung UART
     ----------------
     PA5 = D0 = TxD


     12.09.2018 R. Seelig
   ------------------------------------------------------------------ */

/*
                                                 ATtiny44
                                        4 kByte Flash / 256 Byte SRAM

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

*/

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "usiuart.h"
#include "my_printf.h"

#define  delay                _delay_ms
#define  printf               my_printf

#define cnt_speed    500

#define led_init()     PA1_output_init()
#define led_clr()      PA1_set()
#define led_set()      PA1_clr()



/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  uart_putchar(ch);
}


int main(void)
{
  uint8_t ch;
  uint8_t cx,anz;
  uint8_t ack;


  printfkomma= 3;

  uart_init();
  i2c_master_init();

  while(1)
  {
    printf("\n\rI2C Bus scanning\n\r--------------------------\n\n\r" \
           "Devices found at address:\n\n\r");

    anz= 0;

    for (cx= 0; cx< 254; cx += 2)
    {
      ack= i2c_start(cx);
      delay(1);
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
}
