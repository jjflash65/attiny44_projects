/* ------------------------------------------------------------------
                              dht11_demo.c

     Funktionstest des Luftfeuchtigkeitssensors DHT11

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Hardware zusaetzlich :
             SSD1306 OLED Display

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL

     25.10.2018 R. Seelig
   ------------------------------------------------------------------ */
/*  ------------------------------------------------------------
    Anschlusspins Sensor

    +----------------+       +----------------+
    |                |       |                |
    |                |       |                |
    |     DHT-11     |       |     DHT-11     |
    |                |       |                |
    |    (4 Pin)     |       |     (3 Pin)    |
    |                |       |                |
    +----------------+       +----------------+
     |    |    |    |            |    |    |
     o    o    o    o            o    o    o
    Vcc  Sig  Vcc  Gnd          Sig  Vcc  Gnd

    ------------------------------------------------------------ */

#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"
#include "i2c_sw.h"
#include "my_printf.h"
#include "oled1306_i2c.h"
#include "dht11.h"

#define  delay                _delay_ms
#define  printf               my_printf


/* --------------------------------------------------------
   my_putchar

   wird von my-printf / printf aufgerufen und hier muss
   eine Zeichenausgabefunktion angegeben sein, auf das
   printf dann schreibt !
   -------------------------------------------------------- */
void my_putchar(char ch)
{
  oled_putchar(ch);
}

/* -------------------------------------------------------
                          M-A-I-N
   ------------------------------------------------------- */
int main(void)
{
  int8_t humi, temp;

  ssd1306_init();
  clrscr();

  doublechar= 1;
  gotoxy(0,0);
  printf("DHT-11");
  doublechar= 0;
  gotoxy(0,3);
  printf("Humidity:");
  gotoxy(0,4);
  printf("Temp.:");
  while(1)
  {
    dht_gettemperaturehumidity(&temp, &humi);
    gotoxy(10,3);
    printf("%d %%  ",humi);
    gotoxy(10,4);
    printf("%d %cC  ",temp, 129);
    delay(500);
  }
}
