

/* -------------------------------------------------
     Lookup-table fuer NTC-Widerstand
     R25-Wert: 10.00 kOhm
     Pullup-Widerstand: 4.70 kOhm
     Materialkonstante beta: 3900
     Einheit eines Tabellenwertes: 0.1 Grad Celcius
     Temperaturfehler der Tabelle: 0.4 Grad Celcius
   -------------------------------------------------*/
const int PROGMEM ntctable[] = {
  1622, 1324, 1026, 860, 742, 649, 569, 498, 
  433, 369, 307, 242, 174, 96, 1, -137, 
  -275
};

int ntc_gettemp(uint16_t adc_value)
{
  int p1,p2;

  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.
  p1 = pgm_read_word(&(ntctable[ (adc_value >> 6)    ]));
  p2 = pgm_read_word(&(ntctable[ (adc_value >> 6) + 1]));

  // zwischen beiden Punkten interpolieren.
  return p1 - ( (p1-p2) * (adc_value & 0x003f) ) / 64;
}
