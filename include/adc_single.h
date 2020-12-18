/* ------------------------------------------------------------------
                              adc_single.h

     Header zum Ansprechen des internen AD-Wandlers

     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     13.09.2018 R. Seelig
   ------------------------------------------------------------------ */

#ifndef in_adc_single
  #define in_adc_single

  #include <avr/io.h>


/* --------------------------------------------------------
     Prototypen:

   --------------------------------------------------------
     void adc_init(uint8_t vref, uint8_t channel);

         initialisiert den Analog-Digitalwandler

         Uebergabe:
              vref     0: Vcc; 1= Spg. PA0; 2= 1.1V interne Ref.
              channel  0: PA0 .. 5: PA5

   --------------------------------------------------------
     uint16_t adc_getvalue(void);

         liest einen analogen Wert des aktuell gewaehlten
         Analogeingangs ein

         Rueckgabe:
            10 Bit Integerwert ( 0 .. 1023 )

   -------------------------------------------------------- */

  void adc_init(uint8_t vref, uint8_t channel);
  uint16_t adc_getvalue(void);

#endif
