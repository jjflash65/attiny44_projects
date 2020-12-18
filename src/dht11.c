/*  ------------------------------------------------------------
                                dht11.c

    Softwaremodul zum Auslesen des Hygrometer-Sensors DHT11
    (China-Modul)

    25.10.2018   R. Seelig
    ------------------------------------------------------------ */
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


#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

#include "dht11.h"

/* ----------------------------------------------------------
     dht_getdata

     liest per Bitbanging die Daten des DHT11 Sensors in
     die mit temperature und humidity adressierten Speicher-
     bereiche

     Rueckgabewert:
       0  : erfolgreiches auslesen
       255 : Fehler ist aufgetreten
   ---------------------------------------------------------- */
uint8_t dht_getdata(int8_t *temperature, int8_t *humidity)
{

  uint8_t bits[5];
  uint8_t i,j = 0;

  memset(bits, 0, sizeof(bits));

  // Reset Anschlusspin Sensor
  DHT_DDR |= (1 << DHT_INPUTPIN);                       // Anschluss als Output
  DHT_PORT |= (1 << DHT_INPUTPIN);                      // Pin= 1
  _delay_ms(100);

  // Sendeaufforderung
  DHT_PORT &= ~(1 << DHT_INPUTPIN);                     // Pin = 0
  _delay_ms(18);
  DHT_PORT |= (1 << DHT_INPUTPIN);                      // Pin = 1
  DHT_DDR &= ~(1 << DHT_INPUTPIN);                      // Anschluss als Input
  _delay_us(40);

  // Startkondition insgesamt 2 mal testen
  if((DHT_PIN & (1 << DHT_INPUTPIN)))
  {
    return 255;
  }
  _delay_us(80);

  if(!(DHT_PIN & (1 << DHT_INPUTPIN)))
  {
    return 255;
  }
  _delay_us(80);

  // Sensordaten lesen

  uint16_t timeoutcounter = 0;
  for (j= 0; j < 5; j++)                                // Sensor liefert 5 Datenbytes
  {
    uint8_t result=0;

    for(i= 0; i < 8; i++)
    {
      timeoutcounter = 0;
      while(!(DHT_PIN & (1<<DHT_INPUTPIN)))             // auf 1 Signal Sensor warten
      {
        timeoutcounter++;
        if(timeoutcounter > DHT_TIMEOUT)
        {
          return 255;                                    // Timeout
        }
      }
      _delay_us(30);

      if(DHT_PIN & (1<<DHT_INPUTPIN))                   // 1-Signal laenger als 30 us wird als 1 gelesen
      {
        result |= (1<<(7-i));
      }
      timeoutcounter = 0;

      while(DHT_PIN & (1<<DHT_INPUTPIN))                // auf 0 Signal Sensor warten
      {
        timeoutcounter++;
        if(timeoutcounter > DHT_TIMEOUT)
        {
          return 255;                                    // Timeout
        }
      }
    }
    bits[j] = result;
  }

  // Anschlusspin zuruecksetzen

  DHT_DDR |= (1 << DHT_INPUTPIN);                       // Anschluss als Ausgang
  DHT_PORT |= (1 << DHT_INPUTPIN);                      // Pin= 1;
  _delay_ms(100);

  //Checksummenvergleich
  if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4])
  {
    *temperature = bits[2];
    *humidity = bits[0];
    return 0;
  }
  return 255;
}


/* ----------------------------------------------------------
     dht_gettemperature

     liest die Temperaturwert des DHT11 aus

     Rueckgabewert:
       0  : erfolgreiches auslesen
       255 : Fehler ist aufgetreten
   ---------------------------------------------------------- */
uint8_t dht_gettemperature(int8_t *temperature)
{
  int8_t humidity = 0;

  return dht_getdata(temperature, &humidity);
}

/* ----------------------------------------------------------
     dht_gethumidity

     liest den Luftfeuchtigkeitswert des DHT11 aus

     Rueckgabewert:
       0  : erfolgreiches auslesen
       255 : Fehler ist aufgetreten
   ---------------------------------------------------------- */
uint8_t dht_gethumidity(int8_t *humidity)
{
  int8_t temperature = 0;

  return dht_getdata(&temperature, humidity);
}

/* ----------------------------------------------------------
     dht_gettemperaturehumidity

     liest Temperatur und Luftfeuchtig des Sensors aus

     Rueckgabewert:
       0  : erfolgreiches auslesen
       255 : Fehler ist aufgetreten
   ---------------------------------------------------------- */
uint8_t dht_gettemperaturehumidity(int8_t *temperature, int8_t *humidity)
{
  return dht_getdata(temperature, humidity);
}


