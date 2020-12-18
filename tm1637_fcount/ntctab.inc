

/* -------------------------------------------------
     Lookup-table fuer NTC-Widerstand
     R25-Wert: 10.00 kOhm
     Pullup-Widerstand: 10.00 kOhm
     Materialkonstante beta: 3950
     Aufloesung des ADC: 10 Bit
     Einheit eines Tabellenwertes: 0.1 Grad Celcius
     Temperaturfehler der Tabelle: 0.5 Grad Celcius
   -------------------------------------------------*/
const int PROGMEM ntctable[] = {
  1269, 1016, 763, 621, 520, 439, 370, 308, 
  250, 194, 139, 83, 22, -47, -132, -256, 
  -380
};

/* -------------------------------------------------
                     ntc_gettemp

    zuordnen des Temperaturwertes aus gegebenem
    ADC-Wert.
   ------------------------------------------------- */
int ntc_gettemp(uint16_t adc_value)
{
  int p1,p2;

  // Stuetzpunkt vor und nach dem ADC Wert ermitteln.
  p1 = pgm_read_word(&(ntctable[ (adc_value >> 6)    ]));
  p2 = pgm_read_word(&(ntctable[ (adc_value >> 6) + 1]));

  // zwischen beiden Punkten interpolieren.
  return p1 - ( (p1-p2) * (adc_value & 0x003f) ) / 64;
}
