/* -------------------------------------------------------
                         charlie20.c

     Header fuer Softwaremodul zur Ansteuerung von
     20 LED's mittels Charlieplexing

     Hardware : 20 LED
     MCU      : ATtiny44
     F_CPU    : 8 MHz intern

     Fuses    : fuer 8 MHz intern
                  lo 0xe2
                  hi 0xdf

     Fuses    : fuer ext. Quarz >= 8 MHz
                  lo 0xde
                  hi 0xdf

     18.09.2018    R. Seelig
   ------------------------------------------------------ */

/* -------------------------------------------------------------------------------
                        Charlieplexing 20-LED Modul

     Beim Charlieplexing sind jeweils 2 antiparallel geschaltete Leuchtdioden
     an 2 GPIO-Leitungen angeschlossen. Hieraus ergeben sich 10 Paare zu jeweils
     2 LEDs.

     Bsp.:

      A  o------+-----,         C o------+-----,
                |     |                  |     |
               ---   ---                ---   ---
             A \ /   / \ B            C \ /   / \ D
             B ---   --- A            D ---   --- C
                |     |                  |     |
      B  o------+-----'         D o------+-----'


      A  B  B  C  C  D  D  E  A  C  C  E  D  B  A  D  A  E  E  B   Linenkombination
      B  A  C  B  D  C  E  D  C  A  E  C  B  D  D  A  E  A  B  E
      ----------------------------------------------------------
      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19  LED-Nummern
   ------------------------------------------------------------------------------- */

#include <avr/pgmspace.h>
#include "charlie20.h"


uint32_t      charlie20_buf= 0;                 // Buffer in dem ein Bitmuster aufgenommen wird,
                                                // welches ueber den Timerinterrupt ausgegeben wird

const uint16_t cplex[20] PROGMEM =
//const uint16_t cplex[20] =
  // HiByte: definiert, welche der Charlieplexingleitungen Ausgaenge sind
  // LoByte: definiert, welche Bits gesetzt oder geloescht sind
  //
  //    D15 D14 D13 D12 D11 D10 D09 D08 || D07 D06 D05 D04 D03 D02 D01 D00
  //    ------------------------------------------------------------------
  //                  E   D   C   B   A                  E   D   C   B   A
  //
  // Bsp.: 0x0302

  //   A und B sind Ausgaenge, B= 0, A= 1 => LED AB leuchtet

  {
  //  0=AB    1=BA    2=BC    3=CB    4=CD    5=DC    6=DE    7=ED
    0x0301, 0x0302, 0x0602, 0x0604, 0x0c04, 0x0c08, 0x1808, 0x1810,
  //  8=AC    9=CA   10=CE    11=EC  12=DB   13=BD   14=AD   15=DA
    0x0501, 0x0504, 0x1404, 0x1410, 0x0a08, 0x0a02, 0x0901, 0x0908,
  // 16=AE   17=EA   18=EB    19=BE
    0x1101, 0x1110, 0x1210, 0x1202
  };

/* ------------------------------------------------------
                      TIM0_COMPA_vect

     ISR fuer Timer0 Compare A match.

     Hier werden 20 Bits des Buffers charlie20_buf im
     Zeitmultiplex nacheinander dargestellt.
   ------------------------------------------------------ */
ISR (TIM0_COMPA_vect)
{
  static uint8_t isr_cnt = 0;

  charlie20_lineset(20);                               // alle LED's us
  if (charlie20_buf & ((1ul << isr_cnt)) )             // Bit in charlie20_buf gesetzt ?
  {
    charlie20_lineset(isr_cnt);                        // dann diese LED einschalten
  }
  isr_cnt++;
  isr_cnt= isr_cnt % 20;

  TCNT0= 0;                                   // Zaehlregister zuruecksetzen
}


enum { pscale1 = 1, pscale8, pscale64, pscale256, pscale1024 };
/* -------------------------------------------------------
                        timer0_init

     Initialisiert den Timer0 (8-Bit) als Comparetimer.

     Uebergabe:
        prescale  : Teilerfaktor von F_CPU mit der der
                    Timer getaktet wird.
        compvalue : Vergleichswert, bei dem ein Interrupt
                    ausgeloest wird

     Bsp.:
        F_CPU = 8 MHz, prescaler 1024 = 7812,5 Hz = 128uS Taktzeit Zaehler
        OCR0A = 16: nach 128uS * 16 = 2,048 mS wird ein Interupt ausgeloest (bei
                Startwert TCNT0 = 0).
   ------------------------------------------------------- */
void timer0_init(uint8_t prescale, uint8_t compvalue)
{
  TCCR0B = prescale;
  OCR0A = compvalue;
  TCNT0 = 0;                   // Zaehler zuruecksetzen

  TIMSK0 = 1 << OCIE0A;        // if OCR0A == TCNT0 dann Interrupt
  sei();                       // Interrupts grundsaetzlich an

}

/* ------------------------------------------------------
                   charlie20_allinput

     schaltet alle am Charlieplexing beteiligten I/O
     Leitungen als Eingang
   ------------------------------------------------------ */
void charlie20_allinput(void)
{
  charlieA_input();
  charlieB_input();
  charlieC_input();
  charlieD_input();
  charlieE_input();
}

/* ------------------------------------------------------
                      charlie20_init

     initialisiert die I/O Leitungen des Charlieplexings
     und setzt alle Leitungen auf 1
   ------------------------------------------------------ */
void charlie20_init(void)
{
  charlie20_allinput();

  charlieA_set();
  charlieB_set();
  charlieC_set();
  charlieD_set();
  charlieE_set();

  timer0_init(pscale256, 16);           // F_CPU/256 compare 16 loest alle 0.512ms Interrupt aus
}

/* ------------------------------------------------------
                    charlie20_lineset

     schaltet eine einzelne LED an.

     nr:   Nummer der einzuschaltenden LED (0..19)
           nr== 20 => alle LED's werden ausgeschaltet
   ------------------------------------------------------ */
void charlie20_lineset(char nr)
{
  uint8_t   bl, bh;
  uint16_t  b;

  charlie20_allinput();
  if (nr== 20) return;                               // 20 = alle LED aus

  b= pgm_read_word(&cplex[nr]);
  bh= b >> 8;
  bl= b & 0xff;

  if (bh & 0x01)
  {
    charlieA_output();
    if (bl & 0x01) charlieA_set(); else charlieA_clr();
  }
  if (bh & 0x02)
  {
    charlieB_output();
    if (bl & 0x02) charlieB_set(); else charlieB_clr();
  }
  if (bh & 0x04)
  {
    charlieC_output();
    if (bl & 0x04) charlieC_set(); else charlieC_clr();
  }
  if (bh & 0x08)
  {
    charlieD_output();
    if (bl & 0x08) charlieD_set(); else charlieD_clr();
  }
  if (bh & 0x10)
  {
    charlieE_output();
    if (bl & 0x10) charlieE_set(); else charlieE_clr();
  }
}

