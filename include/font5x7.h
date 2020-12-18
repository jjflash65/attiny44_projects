/*  ---------------------------------------------------------
                         font5x7.h

      Zeichensatz fuer N5110 Display mit PCD8544 Controller
    --------------------------------------------------------- */

#ifndef in_font5x7
  #define in_font5x7

  #include <stdint.h>
  #include <avr/pgmspace.h>


  #define fullascii      1            // 1 : AsciiI Zeichensatz wird auch mit Kleinbuchstaben implementiert
                                      // 0 : Ascii Zeichensatz hoert nach den Grossbuchstaben auf
                                      //     (Speicherplatz spaaren

  #if (fullascii == 1)
    #define lastascii 126             // letztes angegebenes Asciizeichen
  #endif

  #if (fullascii == 0)
    #define lastascii 96
  #endif

  extern const uint8_t fonttab [][5];

#endif
