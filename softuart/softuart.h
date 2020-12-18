/* --------------------------------------------------------
                         softuart.c

     Header fuer eine software realisierte serielle
     Schnittstelle.

     MCU:              ATmega8, ATmegaxx8, ATtinyx5, ATtinyx4

     Timerbenutzung           : TIMER0

     Originalcode von         : Colin Gittins
     AVR-Adaption             : Martin Thomas
     Modifikation 2015..2018  : R. Seelig

     Original-Anmerkungen und Copyrights siehe Dateiende

     13.11.2018
   --------------------------------------------------------*/


#ifndef insoftuart
  #define insoftuart

  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>
  #include <stdio.h>

  #define uartsw_BAUD_RATE    19200


/* --------------------------------------------------------
                         Prototypen
   -------------------------------------------------------- */

  void uartsw_init(void);
  void uartsw_crlf();
  void uartsw_putramstring(char *p);
  void uartsw_putromstring(const unsigned char *dataPtr);

  void uartsw_flush_input_buffer( void );        // Clears the contents of the input buffer.
  unsigned char uartsw_ischar( void );           // Tests whether an input character has been received.
  char uartsw_getchar( void );                   // Reads a character from the input buffer, waiting if necessary.
  unsigned char uartsw_transmit_busy( void );    // To check if transmitter is busy
  void uartsw_putchar( const char );             // Writes a character to the serial port.
  void uartsw_turn_rx_on( void );                // Turns on the receive function.
  void uartsw_turn_rx_off( void );               // Turns off the receive function.
  void uartsw_puts( const char *s );             // Write a NULL-terminated string from RAM to the serial port
  int uartsw_filein(FILE *stream);
  int uartsw_fileout(char ch, FILE *stream);
  static FILE uartsw_io = FDEV_SETUP_STREAM(uartsw_fileout,uartsw_filein,_FDEV_SETUP_RW);


  #define uartsw_prints(string) uartsw_putromstring(PSTR(string))


  #if !defined(F_CPU)
    #warning "F_CPU not defined, set to 8 MHz"
    #define F_CPU 8000000
  #endif

  #if defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__) || defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny44__)

/* --------------------------------------------------------
   Anschlusspins der Software-UART fuer ATtinyX5 / ATtinyx4
   -------------------------------------------------------- */

   #if defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny84__)

      #define uartsw_RXPIN   PINA
      #define uartsw_RXDDR   DDRA
      #define uartsw_RXBIT   PA0

      #define uartsw_TXPORT  PORTA
      #define uartsw_TXDDR   DDRA
      #define uartsw_TXBIT   PA1

      #define uartsw_T_COMP_LABEL      TIM0_COMPA_vect
      #define uartsw_T_COMP_REG        OCR0A
      #define uartsw_T_CONTR_REGA      TCCR0A
      #define uartsw_T_CONTR_REGB      TCCR0B
      #define uartsw_T_CNT_REG         TCNT0
      #define uartsw_T_INTCTL_REG      TIMSK0

      #define uartsw_CMPINT_EN_MASK    (1 << OCIE0A)

      #define uartsw_CTC_MASKA         (1 << WGM01)
      #define uartsw_CTC_MASKB         (0)

      /* "A timer interrupt must be set to interrupt at three times
         the required baud rate." */
      #define uartsw_PRESCALE (8)
      // #define uartsw_PRESCALE (1)

      #if (uartsw_PRESCALE == 8)
          #define uartsw_PRESC_MASKA         (0)
          #define uartsw_PRESC_MASKB         (1 << CS01)
      #elif (uartsw_PRESCALE==1)
          #define uartsw_PRESC_MASKA         (0)
          #define uartsw_PRESC_MASKB         (1 << CS00)
      #else
          #error "prescale unsupported"
      #endif


   #else

      #define uartsw_RXPIN   PINB
      #define uartsw_RXDDR   DDRB
      #define uartsw_RXBIT   PB0

      #define uartsw_TXPORT  PORTB
      #define uartsw_TXDDR   DDRB
      #define uartsw_TXBIT   PB1

// --------------------------------------------------------

      #define uartsw_T_COMP_LABEL      TIM0_COMPA_vect
      #define uartsw_T_COMP_REG        OCR0A
      #define uartsw_T_CONTR_REGA      TCCR0A
      #define uartsw_T_CONTR_REGB      TCCR0B
      #define uartsw_T_CNT_REG         TCNT0
      #define uartsw_T_INTCTL_REG      TIMSK

      #define uartsw_CMPINT_EN_MASK    (1 << OCIE0A)

      #define uartsw_CTC_MASKA         (1 << WGM01)
      #define uartsw_CTC_MASKB         (0)

      /* "A timer interrupt must be set to interrupt at three times
         the required baud rate." */
      #define uartsw_PRESCALE (8)
      // #define uartsw_PRESCALE (1)

      #if (uartsw_PRESCALE == 8)
          #define uartsw_PRESC_MASKA         (0)
          #define uartsw_PRESC_MASKB         (1 << CS01)
      #elif (uartsw_PRESCALE==1)
          #define uartsw_PRESC_MASKA         (0)
          #define uartsw_PRESC_MASKB         (1 << CS00)
      #else
          #error "prescale unsupported"
      #endif

   #endif

  #elif defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324A__)  \
     || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) \
     || defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328PA__) \
     || defined (__AVR_ATmega164P__) || defined (__AVR_ATmega164A__)  \
     || defined (__AVR_ATmega168__)  || defined (__AVR_ATmega88__)    \
     || defined (__AVR_ATmega8__)

/* --------------------------------------------------------
       Anschlusspins der Software-UART fuer ATmega
   -------------------------------------------------------- */

      #define uartsw_RXPIN   PINC
      #define uartsw_RXDDR   DDRC
      #define uartsw_RXBIT   PC5

      #define uartsw_TXPORT  PORTC
      #define uartsw_TXDDR   DDRC
      #define uartsw_TXBIT   PC4

// --------------------------------------------------------

      #define uartsw_T_COMP_LABEL      TIMER0_COMPA_vect
      #define uartsw_T_COMP_REG        OCR0A
      #define uartsw_T_CONTR_REGA      TCCR0A
      #define uartsw_T_CONTR_REGB      TCCR0B
      #define uartsw_T_CNT_REG         TCNT0
      #define uartsw_T_INTCTL_REG      TIMSK0
      #define uartsw_CMPINT_EN_MASK    (1 << OCIE0A)
      #define uartsw_CTC_MASKA         (1 << WGM01)
      #define uartsw_CTC_MASKB         (0)

      /* "A timer interrupt must be set to interrupt at three times
         the required baud rate." */

      #define uartsw_PRESCALE (8)

      // #define uartsw_PRESCALE (1)

      #if (uartsw_PRESCALE == 8)
          #define uartsw_PRESC_MASKA         (0)
          #define uartsw_PRESC_MASKB         (1 << CS01)
      #elif (uartsw_PRESCALE==1)
          #define uartsw_PRESC_MASKA         (0)
          #define uartsw_PRESC_MASKB         (1 << CS00)
      #else
          #error "prescale unsupported"
      #endif
  #else
      #error "no defintions available for this AVR"
  #endif

  #define uartsw_TIMERTOP ( F_CPU/uartsw_PRESCALE/uartsw_BAUD_RATE/3 - 1)

  #if (uartsw_TIMERTOP > 0xff)
      #warning "Check uartsw_TIMERTOP: increase prescaler, lower F_CPU or use a 16 bit timer"
  #endif

  #define uartsw_IN_BUF_SIZE     32

  void uartsw_init(void);                       // Init the Software Uart


#endif
