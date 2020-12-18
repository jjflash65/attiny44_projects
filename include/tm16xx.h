/* ------------------------------------------------------------------
                              tm16xx.h

     Header zum Ansprechen eines TM16xx, Treiberbaustein fuer
     4 und 6 stellige 7-Segmentanzeigen mit gemeinsamer Anode und
     zusaetzlichem Tasteninterface.

     Geeignet fuer: TM1651, TM1637

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf

     Pinbelegung

     ATtiny44     7-Segment Anzeigentreiber
     --------------------------------------
        PA1                 CLK
        PA2                 DIO

     CLK und DIO sind mit jeweils 2,2 kOhm Pop-Up Wider-
     staenden nach +5V zu versehen

     08.11.2019 R. Seelig
   ------------------------------------------------------------------ */

/*
            DIG3  A   F   DIG2  DIG1  B
             _    _    _    _    _    _
          __|_|__|_|__|_|__|_|__|_|__|_|__
         |                                |
         |     DIG3  DIG2   DIG1  DIG0    |
         |      __    __     __    __     |
         |     |__|  |__|   |__|  |__|    |
         |     |__|. |__|.  |__|. |__|.   |
         |                                |
         |           KYX-3461BS           |
         |________________________________|
            |_|  |_|  |_|  |_|  |_|  |_|

             E    D   dp    C    G   DIG0

    Segmentbelegung der Anzeige:

        a
       ---
    f | g | b            Segment |  a  |  b  |  c  |  d  |  e  |  f  |  g  | Doppelpunkt (nur fuer POS1) |
       ---               ---------------------------------------------------------------------------------
    e |   | c            Bit-Nr. |  0  |  1  |  2  |  3  |  4  |  5  |  6  |              7              |
       ---
        d

    Grundsaetzlicher Hinweis:

    An den Anschluss GRID1 des TM16xx wird die hoechstwertigste Anzeige angeschlossen.
    Bei einer 6-stelligen Anzeige ist das DIG5, bei einer 4-stelligen ist das DIG3.
    An einen TM1651 koennen nur max. 4 Digits angeschlossen werden

    -------------------------------------------

    Bsp. Anschluesse fuer TM1651

    TM1651 (Pin)          7-Segmentanzeige (Pin)
    --------------------------------------------

    GRID4    9   ---------------  DIG0       6
    GRID3   10   ---------------  DIG1       8
    GRID2   11   ---------------  DIG2       9
    GRID1   12   ---------------  DIG3      12

    SEG1     2   ---------------    A       11
    SEG2     3   ---------------    B        7
    SEG3     4   ---------------    C        4
    SEG4     5   ---------------    D        2
    SEG5     6   ---------------    E        1
    SEG6     7   ---------------    F       10
    SEG7     8   ---------------    G        5

    VDD     13
    GND      1
    K1      16 (Keyscan 1)
    CLK     15
    DIO     14


    ----------------------------------------------------------

    Bsp. Anschluesse fuer TM1637

    TM1637 (Pin)          7-Segmentanzeige (Pin)
                                4 stellig           6 stellig
    ----------------------------------------------------------

    GRID6   10   ---------------                  |   DIG0
    GRID5   11   ---------------                  |   DIG1
    GRID4   12   ---------------  DIG0       6    |   DIG2
    GRID3   13   ---------------  DIG1       8    |   DIG3
    GRID2   14   ---------------  DIG2       9    |   DIG4
    GRID1   15   ---------------  DIG3      12    |   DIG5

    SEG1     2   ---------------    A       11    |
    SEG2     3   ---------------    B        7    |
    SEG3     4   ---------------    C        4    |
    SEG4     5   ---------------    D        2    |
    SEG5     6   ---------------    E        1    |
    SEG6     7   ---------------    F       10    |
    SEG7     8   ---------------    G        5    |
    SEG8     9   ---------------   dp        3    |

    VDD     16
    GND      1
    K2      20 (Keyscan 2)
    K1      19 (Keyscan 1)
    CLK     18
    DIO     17

*/

#ifndef in_tm16xx
  #define in_tm16xx

  #include <util/delay.h>
  #include <avr/io.h>


  #define enable_6dezdigit          0         // Code fuer 6-stellige dezimale Anzeige einbinden
  #define enable_6dezdigit_nonull   1         // Code fuer 6-stellige dezimale Anzeige mit
                                              // Ausblendung fuehrender Nullen einbinden
  #define enable_6hexdigit          0         // Code fuer 6-stellige hexadezimale Anzeige einbinden


  /* ----------------------------------------------------------
            Anschluss CLK und DIO  an den Controller
     ---------------------------------------------------------- */
  // CLK nach PA1
  #define bb_clkport     A
  #define bb_clkbitnr    1

  // DIO nach PA2
  #define bb_datport     A
  #define bb_datbitnr    2

  // ----------------------------------------------------------------
  // Praeprozessormacros um 2 Stringtexte zur weiteren Verwendung
  // innerhalb des Praeprozessors  zu verknuepfen
  //
  // Bsp.:
  //        #define ionr      A
  //        #define ioport    conc2(PORT, ionr)
  //
  //        ioport wird nun als "PORTA" behandelt
  #define CONC2EXP(a,b)     a ## b
  #define conc2(a,b)        CONC2EXP(a, b)
  // ----------------------------------------------------------------

  // ----------------------------------------------------------------
  //   Makros zum Initialiseren der verwendeten Pins als Ausgaenge
  //   sowie zum Setzen / Loeschen dieser Pins (Bitbanging)
  // ----------------------------------------------------------------

  #define datport           conc2(PORT,bb_datport)
  #define datddr            conc2(DDR,bb_datport)
  #define datpin            conc2(PIN,bb_datport)
  #define clkport           conc2(PORT,bb_clkport)
  #define clkddr            conc2(DDR,bb_clkport)

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

  #define puls_us           5
  #define puls_len()        _delay_us(puls_us)


  /* ----------------------------------------------------------
                       Globale Variable
     ---------------------------------------------------------- */

  extern uint8_t hellig;                  // beinhaltet Wert fuer die Helligkeit (erlaubt: 0x00 .. 0x0f);

  extern uint8_t  led7sbmp[16];           // Bitmapmuster fuer Ziffern von 0 .. F

  /* ----------------------------------------------------------
                           tm16_xofs
       Position, um die eine Anzeigenausgabe verschoben werden
       kann. Wird hier bspw. 2 angegeben, so erscheint die
       Ausgabe fuer Digit 0 nicht an Digit 0, sondern an
       Digit 2, Digit 1 an Digit 3 etc. Dieses dient vor allem
       dafuer, wenn ein 4-stelliger Wert auf einem 6-stelligen
       Display rechts buendig ausgegeben werden soll.
     ---------------------------------------------------------- */
   extern int8_t tm16_xofs;


  /* ----------------------------------------------------------
                           PROTOTYPEN
     ---------------------------------------------------------- */

  void tm16_start(void);
  void tm16_stop(void);
  void tm16_write (uint8_t value);
  uint8_t tm16_read(uint8_t ack);

  void tm16_init(void);
  void tm16_clear(void);
  void tm16_selectDIG(char nr);
  void tm16_setbright(uint8_t value);
  void tm16_setbmp(uint8_t DIG, uint8_t value);
  void tm16_setzif(uint8_t DIG, uint8_t zif);
  void tm16_setseg(uint8_t DIG, uint8_t seg);
  void tm16_setdez(int value, uint8_t dpanz);
  void tm16_sethex(uint16_t value);
  uint8_t tm16_readkey(void);

  void tm16_setdez6digit(uint32_t value, uint8_t dpanz);
  void tm16_setdez6digit_nonull(int32_t value, uint8_t dpanz);
  void tm16_sethex6digit(uint32_t value);

#endif
