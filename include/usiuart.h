/* -----------------------------------------------------------------
                               usiuart.h

     Header fuer Softwaremodul einer USI (Hardware) - gesteuerte serielle
     Schnittstelle

     Verwendet USI-Overflow, Pinchange0 und Timer0 Compare Match
     Interrupt


     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Quellen: AVR App-Note AVR307 und diverse Internetquellen

     17.08.2017 R. Seelig
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

#ifndef in_tx4_usiuart
  #define in_tx4_usiuart


  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <stdbool.h>


  /* -----------------------------------------------------------------------
                       Anschlusspins des USI Interfaces
     ----------------------------------------------------------------------- */
  #define USI_DIR               DDRA
  #define USI_PORT              PORTA
  #define USI_PIN               PINA
  #define USI_DO                PA5               // Anschlusspin fuer TxD
  #define USI_DI                PA6               // Anschlusspin fuer RxD

  #define USI_PCINT             PCINT6            // Pin-Change Interrupt fuer PA6 Anschluss


  /* -----------------------------------------------------------------------
      Moegliche Baudratenkombinationen

      F_CPU 1000000   BAUDRATE 1200, 2400
      F_CPU 8000000   BAUDRATE 9600, 19200
      F_CPU 12000000  BAUDRATE 9600, 19200,
      F_CPU 16000000  BAUDRATE 9600, 19200, 38400
     ----------------------------------------------------------------------- */

  #define BAUDRATE              19200
  #define STOPBITS              1


  /* ------------------------------------------------------------------
                                   PROTOTYPEN
     ------------------------------------------------------------------ */
  void uart_init();
  void uart_putchar(char c);
  uint8_t uart_getchar(void);
  uint8_t uart_ischar(void);

#endif
