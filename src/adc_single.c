/* ------------------------------------------------------------------
                              adc_single.c

     Softwaremodul zum Ansprechen des internen AD-Wandlers

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

#include "adc_single.h"

/* --------------------------------------------------------
                           adc_init

     initialisiert den Analog-Digitalwandler

     Uebergabe:
          vref     0: Vcc; 1= Spg. PA0; 2= 1.1V interne Ref.
          channel  0: PA0 .. 5: PA5


   -------------------------------------------------------- */
void adc_init(uint8_t vref, uint8_t channel)
{
  ADMUX = (vref << 6) | channel;                          // Referenzspannung und Analogeingang waehlen

  ADCSRA = (1 << ADEN) | (1 << ADPS2);                    // ADC enable ADPS2= 1;  ADPS0, ADPS1 = 0
                                                          // => adc-clock = F_CPU / 16
  ADCSRA |= (1 << ADPS1);
  ADCSRB = 0;                                             // ADLAR = 0, ADC wird rechtsbuendig eingelesen
}

uint16_t adc_getvalue(void)
{
  uint16_t result;

  ADCSRA |= 1 << ADSC;                      // ADC starten
  while (ADCSRA & ( 1 << ADSC));            // warten bis ADC fertig ist

  // wichtig: erst niederwertiges Byte lesen
  result = ADCL;
  result |= (uint16_t)(ADCH << 8);
  return result;
}
