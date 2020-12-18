/* ----------------------------------------------------------
                       stepper_motor.c

   Demo zum Betrieb des (China)Schrittmotormoduls

                        28BYJ48

   Schrittmotor hat ein Untersetzungsgetriebe von 1:64, so
   dass fuer eine Umdrehung 4096 Steps benoetigt wird. Die
   Genauigkeit ist allerdings nicht so wirklich hoch
   (China Billigware halt)

   Hardware :  Schrittmotormodul mit Schrittmotor

   MCU      :  Attiny44
   Takt     :  8 MHz intern

   Fuses    :  Lo:0xE2    Hi:0xDF

   14.08.2018 R. Seelig
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
*/


#include <util/delay.h>
#include <avr/io.h>

#include "avr_gpio.h"


#define smo1a_init()    PB0_output_init()
#define smo1a_set()     PB0_set()
#define smo1a_clr()     PB0_clr()

#define smo1b_init()    PB1_output_init()
#define smo1b_set()     PB1_set()
#define smo1b_clr()     PB1_clr()


#define smo2a_init()    PB2_output_init()
#define smo2a_set()     PB2_set()
#define smo2a_clr()     PB2_clr()

#define smo2b_init()    PA7_output_init()
#define smo2b_set()     PA7_set()
#define smo2b_clr()     PA7_clr()


#define delay        _delay_ms
#define rotspeed     1

/* ---------------------------
     Bitsequenz des Motors

     0 0 0 1  = 0x01
     0 0 1 1  = 0x03
     0 0 1 0  = 0x02
     0 1 1 0  = 0x06
     0 1 0 0  = 0x04
     1 1 0 0  = 0x0c
     1 0 0 0  = 0x08
     1 0 0 1  = 0x09
   --------------------------- */
uint8_t smo_seq[8] =
  { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09 };

// Index auf die Bitsequenz des Motors
int8_t smo_index= 0;

/* --------------------------------------------------
                   smo_setnibble

     gibt 4 Bits auf den Anschluessen des Stepper-
     motos aus
   -------------------------------------------------- */
void smo_setnibble(uint8_t value)
{
  if (value & 0x01) smo1a_set(); else smo1a_clr();
  if (value & 0x02) smo1b_set(); else smo1b_clr();
  if (value & 0x04) smo2a_set(); else smo2a_clr();
  if (value & 0x08) smo2b_set(); else smo2b_clr();
}

/* --------------------------------------------------
                   smo_init

     initialisiert die Anschlusesse des Steppermotors
     als Ausgaenge und setzt den Startwert
   -------------------------------------------------- */
void smo_init(void)
{
  smo1a_init();
  smo1b_init();
  smo2a_init();
  smo2b_init();

  smo_setnibble(smo_seq[smo_index]);
}

/* --------------------------------------------------
                   smo_singleleft

     bewegt den Steppermotor um einen Schritt nach
     links
   -------------------------------------------------- */
void smo_singleleft(void)
{
  if (smo_index < 7) smo_index++; else smo_index= 0;
  smo_setnibble(smo_seq[smo_index]);
}

/* --------------------------------------------------
                   smo_singleright

     bewegt den Steppermotor um einen Schritt nach
     rechts
   -------------------------------------------------- */
void smo_singleright(void)
{
  if (smo_index > 0) smo_index--; else smo_index= 7;
  smo_setnibble(smo_seq[smo_index]);
}

/* --------------------------------------------------
                   smo_rotate

     bewegt den Steppermotor um eine vorgegebene
     Anzahl Schritte nach links oder rechts.

     Uebergabe:

           dir   : 0 => Bewegung nach links
                   1 => Bewegung nach rechts
           anz   : Anzahl Schritte die durchgefuehrt
                   werden
   -------------------------------------------------- */
void smo_rotate(uint8_t dir, uint16_t anz)
{
  uint16_t i;

  for (i= 0; i< anz; i++)
  {
    if (dir)
      smo_singleleft();
    else
      smo_singleright();
    delay(rotspeed);
  }
}

/* ---------------------------------------------------------------------
                                  MAIN
   --------------------------------------------------------------------- */
int main(void)
{

  smo_init();
  smo_rotate(1, 4096);              // ganze Drehung nach links
  smo_rotate(0, 4096);              // nach rechts
  smo_setnibble(0);                 // Spulenstrom aus
  while(1);
}
