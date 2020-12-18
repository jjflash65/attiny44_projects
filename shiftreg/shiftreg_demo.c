/* ----------------------------------------------------------
                        shiftreg_demo.c

     Demoprogramm zum Ansteuern eines seriellen
     Schieberegisters (HEF4094 oder SN74HC595).

     MCU       :  Attiny44
     Takt      :  8 MHz intern

     Fuses     :  Lo:0xE2    Hi:0xDF

     Anmerkung : Programm sollte mit allen AVR Controllern
                 laufen, da nur Bitbanging verwendet wird

     20.12.2018  R. Seelig
   ---------------------------------------------------------- */
/*


                                                 ATtiny44
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


                                                 HEF4094
                                             Anschlusspins IC

                                              +------------+
                                              |            |
                                          str | 1   H   16 | +Vcc
                                          dat | 2   E   15 | oe
                                          clk | 3   F   14 | out4
                                         out0 | 4       13 | out5
                                         out1 | 5   4   12 | out6
                                         out2 | 6   0   11 | out7
                                         out3 | 7   9   10 | out_seriell'
                                          gnd | 8   4    9 | out_seriell
                                              |            |
                                              +------------+


                                                 74HC595
                                             Anschlusspins IC

                                              +------------+
                                              |            |
                                         out1 | 1   H   16 | +Vcc
                                         out2 | 2   E   15 | out0
                                         out3 | 3   F   14 | dat
                                         out4 | 4       13 | /oe
                                         out5 | 5   4   12 | strobe
                                         out6 | 6   0   11 | clk
                                         out7 | 7   9   10 | /rst
                                          gnd | 8   4    9 | out7s
                                              |            |
                                              +------------+


               Anschlussbelegung

   ATtiny44            HEF4094
   -------------------------------------
                       16 Vcc
                       15 oe = Vcc
   11 (PA2)    ...      2 (seriell data)
   10 (PA3)    ...      1 (strobe)
   12 (PA1)    ...      3 (clock)
                       10 (seriell out')
                        8 GND

   ATtiny44            SN74HC595
   -------------------------------------
                       16 Vcc
   11 (PA2)    ...     14 (seriell data)
                       13 (/oe) = GND
   10 (PA3)    ...     12 (strobe)
   12 (PA1)    ...     11 (clock)
                       10 (/master reset) = Vcc
                        9 (seriell out; Q7S)
                        8 GND

*/

#include <util/delay.h>
#include <avr/io.h>

// Anbindung Schieberegister an den Mikrocontroller, bei
// Bedarf abaendern


// Dataanschluss nach PA2
#define sr_datport     A
#define sr_datbitnr    2

// Clockanschluss nach PA1
#define sr_clkport     A
#define sr_clkbitnr    1

// Strobeanschluss nach PA3
#define sr_strport     A
#define sr_strbitnr    3


// ----------------------------------------------------------------
// Praeprozessormacros um 2 Stringtexte zur weiteren Verwendung
// innerhalb des Praeprozessors  zu verknuepfen
//
// Bsp.:
//        #define ionr      A
//        #define ioport    conc2(PORT, ionr)
//
//        ioport wird nun als "PORTA" behandelt
#define conc2(a,b)        CONC2EXP(a, b)
#define CONC2EXP(a,b)     a ## b
// ----------------------------------------------------------------

// ----------------------------------------------------------------
//   Makros zum Initialiseren der verwendeten Pins als Ausgaenge
//   sowie zum Setzen / Loeschen dieser Pins (Bitbanging)
// ----------------------------------------------------------------
#define datport           conc2(PORT,sr_datport)
#define datddr            conc2(DDR,sr_datport)
#define clkport           conc2(PORT,sr_clkport)
#define clkddr            conc2(DDR,sr_clkport)
#define strport           conc2(PORT,sr_strport)
#define strddr            conc2(DDR,sr_strport)

#define srdat_init()      datddr|= (1 << sr_datbitnr)
#define srdat_set()       ( datport|= (1 << sr_datbitnr) )
#define srdat_clr()       ( datport&= (~(1 << sr_datbitnr)) )


#define srclk_init()      clkddr|= (1 << sr_clkbitnr)
#define srclk_set()       ( clkport|= (1 << sr_clkbitnr) )
#define srclk_clr()       ( clkport&= (~(1 << sr_clkbitnr)) )

#define srstrobe_init()   strddr|= (1 << sr_strbitnr)
#define srstrobe_set()    ( strport|= (1 << sr_strbitnr) )
#define srstrobe_clr()    ( strport&= (~(1 << sr_strbitnr)) )

// initialisert alle 3 beteiligten Pins als Ausgaenge
#define sr_init()         { srdat_init(); srclk_init(); srstrobe_init(); }


uint8_t sr_value = 0x00;                // Puffervariable des Schieberegisters
                                        // da dieses nicht gelesen werden kann

/* ----------------------------------------------------------
                             sr_setvalue

     transferiert einen 8-Bit Wert ins serielle Schiebe-
     register
   ---------------------------------------------------------- */
void sr_setvalue(uint8_t value)
{
  int8_t i;

  for (i= 7; i> -1; i--)
  {
    if ((value >> i) &1)  srdat_set(); else srdat_clr();

    srclk_set();
    srclk_clr();                            // Taktimpuls erzeugen
  }

  srstrobe_set();                           // Strobeimpuls : Daten Schieberegister ins Ausgangslatch uebernehmen
  srstrobe_clr();
  sr_value= value;
}

/* ----------------------------------------------------------
                             sr_setsinglebit

     setzt ein einzelnes Ausgabnsbit im Schieberegister indem
     die Puffervariable "bitmanipuliert" wird und die Puffer-
     variable ausgegeben wird
   ---------------------------------------------------------- */
void sr_setsinglebit(char nr)
{
  sr_value |= (1 << nr);
  sr_setvalue(sr_value);
}

/* ----------------------------------------------------------
                             sr_clrsinglebit

     loescht ein einzelnes Ausgabnsbit im Schieberegister indem
     die Puffervariable "bitmanipuliert" wird und die Puffer-
     variable ausgegeben wird
   ---------------------------------------------------------- */
void sr_clrsinglebit(char nr)
{
  sr_value &= ~(1 << nr);
  sr_setvalue(sr_value);
}

#define sr_clear()   sr_setvalue(0x00)

#define ledspeed     130

/* ----------------------------------------------------------
                               MAIN
   ---------------------------------------------------------- */
int main(void)
{
  int8_t i;

  sr_init();

  while(1)
  {
    // 8 LED aufblenden
    for (i= 0; i< 8; i++)
    {
      sr_setsinglebit(i);
      _delay_ms(ledspeed);
    }
    // 8 LED abblenden
    for (i= 7; i> -1; i--)
    {
      sr_clrsinglebit(i);
      _delay_ms(ledspeed);
    }

    // Wechselblinker
    for (i= 0; i< 10; i++)
    {
      sr_setvalue(0xaa);
      _delay_ms(ledspeed);
      sr_setvalue(0x55);
      _delay_ms(ledspeed);
    }
    sr_clear();
  }
}
