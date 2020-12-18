/* ----------------------------------------------------------
                           mini_io.h


     Header Softwaremodul fuer ein kombiniertes

                     "Minimum I/O System"

     Beinhaltet Software fuer eine kombinierte gemultiplexte
     zweistellige LED 7-Segmentanzeige mit maximal zu-
     saetzlichen 8 einzelnen LED's.
     Die Daten fuer die LED-Anzeigen werden ueber ein
     SN74HC595 Schieberegister ausgegeben.
     Alle LED's (auch die der einzelnen) sind als gemein-
     same Kathode implementiert.

     Desweiteren existiert ein Lautsprecheranschluss mittels
     diesem  einfache monophonic Toene erzeugt werden koennen.

     Zusaetzlich in diesem Softwaremodul existiert die
     Abfragemoeglichkeit von 4 User-Tastern.

     Anmerkung: Dadurch, dass mit Timer0 die Frequenzer-
     zeugung des Tons gehandhabt wird und in derselben
     ISR das Multiplexing der Anzeige stattfindet, ist
     die Multiplexfrequenz der Anzeige abhaengig vom ge-
     rade eingestellten Ton.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     30.10.2018  R. Seelig

   ---------------------------------------------------------- */

/*

                                                ATtiny44
                                              +-----------+
                                         Vcc  | 1      14 |  GND
                   PCINT8 - XTAL1 - CLKI PB0  | 2   A  13 |  PA0 - ADC0 - AREF - PCINT0
                        PCINT9 - XTAL2 - PB1  | 3   T  12 |  PA1 - ADC1 - AIN0 - PCINT1
                 PCINT11 - /reset - dW - PB3  | 4   t  11 |  PA2 - ADC2 - AIN1 - PCINT2
         PCINT10 - INT0 - OC0A - CKOUT - PB2  | 5   i  10 |  PA3 - ADC3 - T0 - PCINT3
            PCINT7 - ICP - OC0B - ADC7 - PA7  | 6   n   9 |  PA4 - ADC4 - USCK - SCL - T1 - PCINT4
PCINT6 - OC1A - SDA - MOSI - DI - ADC6 - PA6  | 7   y   8 |  PA5 - ADC5 - DO - MISO - OC1B PCINT5
                                              +-----------+



                                               SN74HC595
                                              +----------+
                                           Q1 | 1     16 | Vcc
                                           Q2 | 2     15 | Q0
                                           Q3 | 3     14 | ser_data
                                           Q4 | 4     13 | /oe
                                           Q5 | 5     12 | strobe (clk_latch)
                                           Q6 | 6     11 | clk_ser_data
                                           Q7 | 7     10 | /master_clr
                                          GND | 8      9 | Q7S (register overflow)
                                              +----------+



                              2-Digit 7-Segmentanzeige 7 gemeinsame Kathode
                                            f  GK1  b   a  GK2
                                            _   _   _   _   _
                                           | | | | | | | | | |
                                         +---------------------+
                                         |    _a_       _a_    |
                                         |  f|   |b   f|   |b  |
                                         |   |_g_|     |_g_|   |
                                         |  e|   |c   e|   |c  |
                                         |   |___| .   |___| . |
                                         |     d         d     |
                                         +---------------------+
                                           |_| |_| |_| |_| |_|
                                            e   d   c   g   dp



                               --------------------------------------------
                                                Verdrahtung
                               --------------------------------------------


                                       ATtiny44            SN74HC595
                                   -------------------------------------
                                                           16 Vcc

                                       12 (PA1)    ...     14 (seriell data)
                                                           13 (/oe) = GND
                                       11 (PA2)    ...     12 (strobe)
                                       13 (PA0)    ...     11 (clock)
                                                           10 (/master reset) = Vcc

                                                            8 GND
                    Lautsprecher  ...   2 (PB0)
                         MPX LED  ...   8 (PA5)
                         MPX LSD  ...   9 (PA4)
                         MPX MSD  ...  10 (PA3)



                         SN74HC595           2-Digit 7-Segmentanzeige        single LED
                      --------------------------------------------------------------------
                         15 (Q0)     ...     seg_a           ...             LED0
                          1 (Q1)     ...     seg_b           ...             LED1
                          2 (Q2)     ...     seg_c           ...             LED2
                          3 (Q3)     ...     seg_d           ...             LED3
                          4 (Q4)     ...     seg_e           ...             (optional LED4)
                          5 (Q5)     ...     seg_f           ...             (optional LED5)
                          6 (Q6)     ...     seg_g           ...             (optional LED6)
                          7 (Q7)                             ...             (optional LED7)


*/

#ifndef in_mini_io
  #define in_mini_io

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>

  #include "avr_gpio.h"

  #ifndef F_CPU
    #error Keine F_CPUtaktangabe, kann Timerreload nicht bestimmen
  #endif


  /* ---------------------------------------------------------------------
       Anschlusspins
     -------------------------------------------------------------------- */

  //  Anschlusspins des SN74HC595

  #define srclock_init()      PA0_output_init()
  #define srclock_set()       PA0_set()
  #define srclock_clr()       PA0_clr()

  #define srdata_init()       PA1_output_init()
  #define srdata_set()        PA1_set()
  #define srdata_clr()        PA1_clr()

  #define srstrobe_init()     PA2_output_init()
  #define srstrobe_set()      PA2_set()
  #define srstrobe_clr()      PA2_clr()

  //  Multiplexpins fuer 7-Segmentanzeige und LED-Reihe

  #define mpx0_init()         PA3_output_init()                   // MPX MSD 7-Segment
  #define mpx0_set()          PA3_set()
  #define mpx0_clr()          PA3_clr()

  #define mpx1_init()         PA4_output_init()                   // MPX LSD 7-Segment
  #define mpx1_set()          PA4_set()
  #define mpx1_clr()          PA4_clr()

  #define mpx2_init()         PA5_output_init()                   // MPX LED
  #define mpx2_set()          PA5_set()
  #define mpx2_clr()          PA5_clr()

  // Anschlusspin Lautsprecher

  #define spk_init()          PB0_output_init()
  #define setspk()            PB0_set()
  #define clrspk()            PB0_clr()

  // Anschlusspins Tasten

  #define but3_init()         PB1_input_init()
  #define but2_init()         PB2_input_init()
  #define but1_init()         PA6_input_init()
  #define but0_init()         PA7_input_init()
  #define button_init()       { but0_init(); but1_init(); but2_init(); but3_init(); }

  #define is_but3()           (!(is_PB1()))
  #define is_but2()           (!(is_PB2()))
  #define is_but1()           (!(is_PA7()))
  #define is_but0()           (!(is_PA6()))

  /* ---------------------------------------------------------------------
       Defines
     -------------------------------------------------------------------- */

  #define startreload 833     // Startwert des Timer-Reloadwertes
  #define playtempo 5         // Spielgeschwindigkeit beim Abspielen Notenstring

  /* ---------------------------------------------------------------------
       Variable
     -------------------------------------------------------------------- */

  extern uint8_t           digit2_outmode;             // 0 : Ziffernausgabe
                                                       // 1 : Ansteuerung einzelner Segmente (mit Bitmuster in
                                                       //     digit2_segvalues)

  extern uint16_t          digit2_segvalues;           // beinhaltet bei Segmentausgabe das auszugebende Bitmuster
  extern uint8_t           led_anzbuf;                 // Buffer der anzuzeigenden einzelnen LEDs

  extern volatile uint8_t  sound;                      // schaltet Ton an/aus
  extern int               freqreload [24];            // beinhaltet Frequenzen fuer C-Dur Toene


  /* #####################################################################
       Prototypen
     ##################################################################### */

  // Funktionen 2-stellige 7-Segmentanzeige

  void digit2_init(void);
  void digit2_dezout(uint8_t value);
  void digit2_hexout(uint8_t value);

  // Funktionen Tonerzeugung

  void settonfreq(uint16_t wert);
  void tonlen(int w);
  void playnote(char note);
  void playstring(const unsigned char* const s);

  // Funktionen Taster

  uint8_t button_get(void);


  // Funktion Timer

  void timer0_init(void);

#endif
