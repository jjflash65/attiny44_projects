/* ------------------------------------------------------------------
                              tm1638.h

    Header fuer Softwaremodul zu Chinaboards mit TM1638 7-Segment-
    anzeigebaustein.

    Derzeit (Stand Maerz 2019) existieren 2 Boards, eines mit 8,
    ein anderes mit 16 Tasten und leider ist das Ansprechen der
    Boards verschieden, denn:

    - auf dem Board mit 8 Tasten wurden Anzeigen mit gemeinsamer
      Kathode und zusaetzlichen 8 Einzel-LEDs verbaut

    - auf dem Board mit 16 Tasten wurden Anzeigen mit gemeinsamer
      Anode verbaut

    Hieraus ergibt sich die Notwendigkeit unterscheiden zu muessen,
    fuer welches der Boards die vorliegende Software eingesetzt
    wird.

    Beim Board mit 8 Tasten gibt es zusaetzlich Funktionen zum Setzen
    der Einzel_LEDs sowie das Lesen einer Tastenmatrix (welche beim
    Board mit 16 Tasten aufgrund der anderen Verschaltung nicht
    moeglich ist).


    MCU : alle AVR

    07.03.2019  R. Seelig
   ------------------------------------------------------------------ */

#ifndef in_tm1638
  #define in_tm1638

  #include <string.h>

  #include <avr/io.h>
  #include <util/delay.h>
  #include <avr/pgmspace.h>
  #include "avr_gpio.h"


  #define board_version                           1      // 1 => Board mit 8 Tasten und zusaetzlichen 8 Einzel-LED
                                                         // 2 => Board mit 16 Tasten

  /* ----------------------------------------------------------
         Anschluss CLK und DIO des TM1638 an den Controller
     ---------------------------------------------------------- */
  // DIO nach PB1
  #define bb_datport       B
  #define bb_datbitnr      1

  // CLK nach PB0
  #define bb_clkport       B
  #define bb_clkbitnr      0

  // STB nach PA5
  #define bb_stbport       A
  #define bb_stbbitnr      5


  /* ----------------------------------------------------------
                  einbinden optionale Funktionen
     ---------------------------------------------------------- */
  #define readint_enable   1                      // 1 = Funktion Integerzahl einlesen einbinden
                                                  // 0 = disable

  /* ----------------------------------------------------------
                           globale Variable
     ---------------------------------------------------------- */
  extern uint8_t fb1638[8];                       // der Framebuffer fuer die Anzeige
  extern uint8_t tm1638_brightness;               // Puffervariable zum Setzen der Helligkeit

  /* ----------------------------------------------------------
                            Prototypen
     ---------------------------------------------------------- */

     // Framebuffer -funktionen und -makros
  #define  fb1638_clr()       memset(fb1638, 0, 8)
  void     fb1638_setbmp(uint8_t b, uint8_t pos);
  void     fb1638_putseg(uint8_t pos, uint8_t setrst);
  void     fb1638_setchar(uint8_t ch, uint8_t pos);
  void     fb1638_puts(const char *s, char pos);
  #define  fb1638_prints(s, pos)    fb1638_puts(PSTR(s), pos)

  // Chipfunktionen
  void     tm1638_init(void);
  void     tm1638_wradr(uint8_t adr, uint8_t value);
  void     tm1638_clear(void);
  void     tm1638_showbuffer(void);
  void     tm1638_setdp(uint8_t pos, uint8_t enable);
  void     tm1638_setdez(int32_t value, uint8_t pos, uint8_t nozero);
  void     tm1638_sethex(int32_t value, uint8_t pos, uint8_t nozero);
  uint8_t  tm1638_readkeys(void);

  #if (board_version == 1)
    void tm1638_setled(uint8_t value);
    uint8_t tm1638_readkeymatrix(void);
  #endif

  #if ((readint_enable == 1) && (board_version == 2))
    extern const uint8_t calckeymap [16] PROGMEM;
    uint8_t tm1638_readint(uint32_t *value);
  #endif


  // ----------------------------------------------------------------
  // Praeprozessormacros um 2 Stringtexte zur weiteren Verwendung
  // innerhalb des Praeprozessors  zu verknuepfen
  //
  // Bsp.:
  //        #define ionr      A
  //        #define ioport    CONCAT(PORT, ionr)
  //
  //        ioport wird nun als "PORTA" behandelt
  #define CONC2EXP(a,b)     a ## b
  #define CONCAT(a,b)       CONC2EXP(a, b)
  // ----------------------------------------------------------------

  // ----------------------------------------------------------------
  //   Makros zum Initialiseren der verwendeten Pins als Ausgaenge
  //   sowie zum Setzen / Loeschen dieser Pins (Bitbanging)
  // ----------------------------------------------------------------

  #define datport           CONCAT(PORT,bb_datport)
  #define datddr            CONCAT(DDR,bb_datport)
  #define datpin            CONCAT(PIN,bb_datport)

  #define clkport           CONCAT(PORT,bb_clkport)
  #define clkddr            CONCAT(DDR,bb_clkport)

  #define stbport           CONCAT(PORT,bb_stbport)
  #define stbddr            CONCAT(DDR,bb_stbport)

  // ----------------------------------------------------------------
  //   Anmerkung zum Setzen von 1 und 0 auf den Pins
  //
  //   Die Pins sind in der Grundkonfiguration als Eingang geschaltet.
  //   Beim Setzen einer 1 wird nur die Konfiguration des Pins als
  //   Eingang benoetigt, da dieser dann hochohmig ist und die Leitung
  //   ueber den Pull-Up Widerstand auf 1 gelegt wird.
  //   Bei der Ausgabe einer 0 wird der Pin als Ausgang konfiguriert
  //   und dieser Pin mit einer 0 beschrieben
  // ----------------------------------------------------------------

  #define sda_init()        datddr &= ~(1 << bb_datbitnr)
  #define bb_sda_hi()       sda_init()
  #define bb_sda_lo()       { datddr |= (1 << bb_datbitnr);  datport&= (~(1 << bb_datbitnr)); }
  #define bb_is_sda()       (( datpin & ( 1 << bb_datbitnr) ) >> bb_datbitnr )

  #define scl_init()        datddr &= ~(1 << bb_clkbitnr)
  #define bb_scl_hi()       scl_init()
  #define bb_scl_lo()       { clkddr |= (1 << bb_clkbitnr);  clkport&= (~(1 << bb_clkbitnr)); }

  #define stb_init()        stbddr |= 1 << bb_stbbitnr
  #define bb_stb_hi()       stbport |= 1 << bb_stbbitnr
  #define bb_stb_lo()       stbport &= ~(1 << bb_stbbitnr)

  #define puls_us           10
  #define puls_len()        _delay_us(puls_us)

#endif
