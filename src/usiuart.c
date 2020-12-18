/* ------------------------------------------ -----------------------
                             usiuart.c

     Softwaremodul fuer eine USI (Hardware) - gesteuerte serielle
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

#include "usiuart.h"


/* -----------------------------------------------------------------------
   Prescaler Berechnung in Abhaengigkeit der Taktrate und clockselect setzen
   F_CPU muss ueber das Makefile definiert sein, ansonsten hier einen Wert
   fuer F_CPU angeben
   ----------------------------------------------------------------------- */

#define CYCLES_PER_BIT        ( (F_CPU) / (BAUDRATE) )
#if (CYCLES_PER_BIT > 255)
  #define DIVISOR             8
  #define CLOCKSELECT         2
#else
  #define DIVISOR             1
  #define CLOCKSELECT         1
#endif

#define FULL_BIT_TICKS        ( (CYCLES_PER_BIT) / (DIVISOR) )
#define HALF_BIT_TICKS        ( FULL_BIT_TICKS / 2)

// Anzahl der Takte nach Eingang eines Pin-Changes bis zum Start des USI-Timers
#define START_DELAY           (99)

// Anzahl der Takte die vergehen, nachdem Interrupt aktiviert ist
#define COMPA_DELAY           42
#define TIMER_MIN ( COMPA_DELAY / DIVISOR )

#define TIMER_START_DELAY     ( START_DELAY  / DIVISOR )

#if (HALF_BIT_TICKS - TIMER_START_DELAY)>0

  #define TIMER_TICKS         ( HALF_BIT_TICKS - TIMER_START_DELAY )

  #if (TIMER_TICKS < TIMER_MIN)
    #warning TIMER_TICKS zu langsam, niedrigere Baudrate waehlen
  #endif

#else

  #error "TIMER_TICKS unzulaessige Werte: F_CPU, BAUDRATE and START_DELAY muessen unterschiedliche Werte besitzen"
  #define TIMER_TICKS         1

  #endif


// Enumerator Statusvariable send
enum USISERIAL_SEND_STATE { AVAILABLE, FIRST, SECOND };

// Enumerator UART-Funktion
enum UART_FUNC { TXD, RXD };

/* ------------------------------------------------------------------------
                                    Variable
   ------------------------------------------------------------------------ */

static volatile enum UART_FUNC uart_func = RXD;
static volatile enum USISERIAL_SEND_STATE usiserial_send_state = AVAILABLE;
static volatile uint8_t usiserial_tx_data;

volatile bool usiserial_readfinished= true;
volatile bool serialdataready = false;                  // zeigt an, ob ein Datum eingegangen ist
volatile uint8_t serialinput;                           // letztes eingegangenes Datum


/* ------------------------------------------------------------------------
         Funktionen die einzelne Flags setzen oder Funktionen wrappen
   ------------------------------------------------------------------------ */

static inline enum USISERIAL_SEND_STATE usiserial_send_get_state(void)
{
  return usiserial_send_state;
}

static inline void usiserial_send_set_state(enum USISERIAL_SEND_STATE state)
{
  usiserial_send_state=state;
}

bool usiserial_send_available()
{
  return usiserial_send_get_state() == AVAILABLE;
}

static inline uint8_t usiserial_get_tx_data(void)
{
  return usiserial_tx_data;
}

static inline void usiserial_set_tx_data(uint8_t tx_data)
{
  usiserial_tx_data = tx_data;
}


/* ------------------------------------------------------------------
                             reverse_byte

                 Bit MSB nach LSB und umgekehrt

       Bsp: aus 0x03 wird 0xC0
   ------------------------------------------------------------------ */

static uint8_t reverse_byte (uint8_t x)
{
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}


/* ------------------------------------------------------------------
                          usiserial_send_byte
   ------------------------------------------------------------------ */
void usiserial_send_byte(uint8_t data)
{
  while (usiserial_send_get_state() != AVAILABLE);      // wiederholen bis vorrausgegangener Frame gesendet ist

  usiserial_send_set_state(FIRST);
  usiserial_set_tx_data(reverse_byte(data));

  // Konifguration Timer0
  TCCR0A = 2 << WGM00;                                  // CTC mode
  TCCR0B = CLOCKSELECT;                                 // Rrescaler auf clk oder clk / 8 setzen
  GTCCR |= 1 << PSR10;                                  // Reset prescaler
  OCR0A = FULL_BIT_TICKS;                               // Trigger
  TCNT0 = 0;                                            // Count up from 0

  // Konfiguration USI, Startbit senden und 7 Datenbits

  USIDR = 0x00 |                                        // Start bit (low)
      usiserial_get_tx_data() >> 1;                     // gefolgt von 7 Datenbits

  USICR  = (1 << USIOIE)|                               // Interrupt zulassen
      (0 << USIWM1) | (1 << USIWM0) |                   // 3 - wire-modus
      (0 << USICS1) | (1 << USICS0) | (0<<USICLK);      // Timer0 Compare match Modus als USI Taktquelle

  USI_DIR  |= (1 << USI_DO);                            // Senden - Pin als Ausgang setzen
  USISR = 1 << USIOIF |                                 // Interruptflag loeschen
      (16 - 8);                                         // und Zaehler fuer 8 Bits
}

/* ------------------------------------------------------------------
                             serialreceived

     setzt Flag, dass ein Datum eingegangen ist
     und speichert dieses Datum
   ------------------------------------------------------------------ */
void serialreceived(uint8_t data)
{
  serialdataready = true;
  serialinput = data;
}


/* ------------------------------------------------------------------
                          on_serial_pinchange

     Timer nach erstem Pinchange auf Baudrate konfigurieren, starten und weitere
     Pinchanges bis zum Ende des Frames verhindern.
   ------------------------------------------------------------------ */
void on_serial_pinchange()
{
  GIMSK &= ~(1 << PCIE0);                               // Disable pin change interrupt
  TCCR0A = 2 << WGM00;                                  // CTC mode
  TCCR0B = CLOCKSELECT;                                 // Taktvorteiler
  GTCCR |= 1 << PSR10;                                  // Prescaler reset
  OCR0A = TIMER_TICKS;                                  // Die Zeit, an der der RxD Pin gelesen wird auf die (zeitliche) Mitte des Bits setzen
  TCNT0 = 0;
  TIFR0 = 1 << OCF0A;                                   // Output Compareflag loeschen
  TIMSK0 |= 1 << OCIE0A;                                // und Timerinterrupts zulassen
}


/* ------------------------------------------------------------------
                       Pinchange Interruptvektor

     reakiert auf alle "Veraenderungen" an den Anschluessen des PortA
     und detektiert somit das Startbit. Mit dem Aufruf von
     on_serial_pinchange wird dieser Interrupt fuer den Rest des
     seriellen Datenframes gesperrt
   ------------------------------------------------------------------ */
ISR (PCINT0_vect)
{
  uint8_t pinbVal;

  usiserial_readfinished= false;

  pinbVal = USI_PIN;
  if (!(pinbVal & 1 << USI_DI))                         // wird nur eingelesen, wenn DI == 0
  {
    on_serial_pinchange();
  }
}


/* ------------------------------------------------------------------
                Timer 0 Compare Match Interruptvektor

     startet in der zeitlichen Mitte von Bit 0
   ------------------------------------------------------------------ */
ISR (TIM0_COMPA_vect)
{
  TIMSK0 &= ~(1 << OCIE0A);                             // COMPA sperren
  TCNT0 = 0;                                            // Zaehler auf 0
  OCR0A = FULL_BIT_TICKS;                               // einzelne zeitliche Bitbreite

  // USI-Overflow zulassen, Timer 0 ist Taktquelle fuer USI
  USICR = 1 << USIOIE | 0 << USIWM0 | 1 << USICS0;

  // Resetr Start Kondition Interrupt Flag, USI OVF flag, Counter auf 8 setzen
  USISR = 1 << USIOIF | 8;
}


/* ------------------------------------------------------------------
                     USI Overflow Interruptvektor

     USI overflow interrupt fuer senden UND empfangen
   ------------------------------------------------------------------ */
ISR (USI_OVF_vect)
{
  uint8_t temp = USIBR;

  cli();

  if (uart_func == TXD)
  {
    if (usiserial_send_get_state() == FIRST)
    {
      usiserial_send_set_state(SECOND);

      USIDR = usiserial_get_tx_data() << 7             // letztes Bit (LSB) des Datums senden
          | 0x7F;                                      // plus Stopbit

      USISR = 1 << USIOIF |                            // Interrupt quittieren
          (16 - (1+ (STOPBITS)));                      // USI counter setzen um letztes Daten Bit + Stopbit zu senden
    }
    else
    {
      USI_PORT |= 1 << USI_DO;                         // USI_DO = 1
      USI_DIR  |= (1 << USI_DO);                       // USI_DO als Ausgang
      USICR = 0;                                       // USI disable
      USISR |= 1 << USIOIF;                            // Interrupt quittieren

      usiserial_send_set_state(AVAILABLE);
    }
  }
  else
  {
    USICR  =  0;                                       // Disable USI

    serialreceived(reverse_byte(temp));

    GIFR = 1 << PCIF0;                                 // Pinchange Interrupt Flag quittieren
    GIMSK |= 1 << PCIE0;                               // und Pinchange Interrupts wieder zulassen

    usiserial_readfinished= true;
    USISR |= 1 << USIOIF;                              // Interrupt quittieren
  }

  sei();
}

/* ------------------------------------------------------------------
                                 uart_init

     initialisiert das USI Interface als serielle Schnittstelle

     TxD - USI_DO
     RxD - USI_DI

     (Hinweis: nicht von den Anschluessen des SPI zum Flashen des
      AVR's verwirren lassen !!!)

     Aktivieren des Pinchange-Interrupts
   ------------------------------------------------------------------ */
void uart_init()
{
  USI_PORT |= ( 1 << USI_DI );      // Pull-Up Widerstand fuer Rx-Data
  USI_DIR &= ~( 1 << USI_DI );
  USI_DIR |= ( 1 << USI_DO );
  USI_PORT |= ( 1 << USI_DO );

  USICR = 0;                        // Disable USI.
  GIFR = 1 << PCIF0;                // Clear pin change interrupt flag (PCINT0--PCINT7)
  GIMSK |= 1 << PCIE0;              // Enable pin change interrupts (PCINT0--PCINT7)
  PCMSK0 |= 1 << USI_PCINT;         // Enable pin change on pin PA6

  sei();
}

/* ------------------------------------------------------------------
                              uart_putchar

     schreibt ein Zeichen auf der USI-Schnittstelle, die als UART
     konfiguriert ist. TxD ist Anschluss D0 (entspricht wirklich
     MISO Anschluss der SPI Schnittstelle)
   ------------------------------------------------------------------ */
void uart_putchar(char c)
{
  uart_func= TXD;
  while (!usiserial_send_available());
  usiserial_send_byte(c);
  while (!usiserial_send_available());
  uart_func= RXD;
}


/* ------------------------------------------------------------------
                              uart_getchar

     liest ein Zeichen auf der USI-Schnittstelle ein, die als UART
     konfiguriert ist. RxD ist Anschluss DI (das entspricht wirklich
     MOSI - Anschluss der SPI Schnittstelle)
   ------------------------------------------------------------------ */
uint8_t uart_getchar(void)
{
  uint8_t ch;

  while(!(serialdataready))
  while(!(usiserial_readfinished));
  serialdataready= false;
  ch= serialinput;

  _delay_us((100000 / (BAUDRATE / 100)));       // warten bis 10 Bits (Startbit, Stopbit, 8 Datenbits) eingelesen sind

  return ch;
}

/* ------------------------------------------------------------------
                              uart_ischar

     testet, ob auf der USI-Schnittstelle die als UART konfiguriert
     ist, ein Zeichen eingetroffen ist. Hierbei wird dieses Zeichen
     jedoch NICHT gelesen.
   ------------------------------------------------------------------ */
uint8_t uart_ischar(void)
{
  if (serialdataready) return 1; else return 0;
}
