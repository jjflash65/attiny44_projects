/* -------------------- ---------------------------------
                          toene.h

     Header-Datei fuer eine "Abspielfunktion"
     zum Abspielen eines Notenstrings oder zum generieren
     Timerinterruptgesteuerter Frequenzen.

     Verwendet 8-Bit Timer0 zum Abspielen sodass der
     16-Bit Timer einer evtl. Software Uhr vorbehalten
     bleibt.

     Hardware:
        MCU     : ATtiny44

     Compiler: AVR-GCC 4.3.2

     18.06.2014        R. Seelig

  -------------------------------------------------------- */

#ifndef in_toene_tim0
  #define in_toene_tim0

  #include <util/delay.h>                                    // beinhaltet _delay_ms(char) und _delay_us(char)
  #include <avr/interrupt.h>
  #include <avr/pgmspace.h>


  // ------------------ Speaker ----------------------------

  // Beispielanschluss an einem ATtiny44

  #define speakerdir       DDRA
  #define speakerport      PORTA
  #define speakerpin       PA7


  #define setspk()         (speakerport |= (1 << speakerpin))
  #define clrspk()         (speakerport &= ~(1 << speakerpin))


  #ifndef F_CPU
    #error Keine F_CPUtaktangabe, kann Timerreload nicht bestimmen
  #endif

  #define startreload 833
  #define playtempo 5

  extern uint8_t togglespk;
  extern volatile uint8_t sound;
  extern volatile uint8_t cnt;
  extern volatile uint8_t srout;

  extern int freqreload [24];

  ISR (TIMER0_COMPA_vect);
  void toene_init(void);
  void spk_delay(int wert);
  void spk_puls(int wert);
  void settonfreq(uint16_t wert);
  void tonlen(int w);
  void playnote(char note);
  void playstring(const unsigned char* const s);


#endif
