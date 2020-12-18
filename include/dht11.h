/*  ------------------------------------------------------------
                              dht11.h

    Header Softwaremodul zum Auslesen des Hygrometer-Sensors
    DHT11 (China-Modul)

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


#ifndef DHT_H_
  #define DHT_H_

  #include <avr/io.h>

  // Portpin an dem der Sensor angeschlossen ist

  #define DHT_DDR DDRA
  #define DHT_PORT PORTA
  #define DHT_PIN PINA
  #define DHT_INPUTPIN PA0

  // Anzahl Versuche den Sensor zu lesen bis Timeout erfolgt
  #define DHT_TIMEOUT 200

  // Prototypen
  uint8_t dht_gettemperature(int8_t *temperature);
  uint8_t dht_gethumidity(int8_t *humidity);
  uint8_t dht_gettemperaturehumidity(int8_t *temperature, int8_t *humidity);

#endif
