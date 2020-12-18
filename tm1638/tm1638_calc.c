/* ------------------------------------------------
                   tm1638_tst.c


     01.02.2019
   ----------------------------------------------- */

// #define F_CPU 16000000ul       // 16 MHz Taktfrequenz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "tm1638.h"

#define delay(anz)           _delay_ms(anz)

const uint8_t lauflseq [20] PROGMEM =
{ 0,1,2,3,4,5,6,7, 47, 39, 31,30,29,28,27,26,25,24,
  16, 8 };


/*  -----------------------------------------------------------------------------
                                          MAIN
    -----------------------------------------------------------------------------  */
int main(void)
{
  uint8_t i, k;
  uint32_t op1, op2;
  uint8_t op1read;
  uint8_t cmd;

  tm1638_init();
  tm1638_brightness= 3;

  // Demo einzelne Segmente im Lauflicht auf- und abblenden
  fb1638_clr();
  for (i= 0; i< 20; i++)
  {
    k= pgm_read_byte(&lauflseq[i]);
    fb1638_putseg(k,1);
    tm1638_showbuffer();
    delay(70);
  }
  for (i= 0; i< 20; i++)
  {
    k= pgm_read_byte(&lauflseq[i]);
    fb1638_putseg(k,0);
    tm1638_showbuffer();
    delay(70);
  }
  delay(200);

  // Demo Text einscrollen
  for (i= 0; i< 8; i++)
  {
    fb1638_clr();
    fb1638_prints("1nt CALC",i);
    tm1638_showbuffer();
    delay(100);
  }

  // und Dezimalpunkt setzen
  tm1638_setdp(5,1);
  delay(1300);

  // Demo Integer Calculator

  op1read= 1;
  while(1)
  {
    if (op1read)
    {
      do
      {
        op1= 0;
        cmd= tm1638_readint(&op1);
        tm1638_clear();
        delay(200);
        tm1638_setdez(op1,0,1);
      } while (cmd != 0x0d);          // erster Operand muss mit Enter abgeschlossen sein
      op1read= 0;
    }
    op2= op1;
    do
    {
      cmd= tm1638_readint(&op2);
      tm1638_clear();
      delay(200);
      if (cmd == 0x0d) op2= 0;
    } while(cmd == 0x0d);             // zweiter Operand darf nicht mit ENTER abgeschlossen sein

    switch (cmd)
    {
      case 0x18 :                     // clr
      {
        op1= 0; op2= 0; op1read= 1;   // Operande loeschen und flag fuer ersten Operanten lesen
        break;
      }
      case '+' :
      {
        op1 += op2;
        tm1638_setdez(op1,0,1);
        break;
      }
      case '-' :
      {
        op1 -= op2;
        tm1638_setdez(op1,0,1);
        break;
      }
      case '*' :
      {
        op1 *= op2;
        tm1638_setdez(op1,0,1);
        break;
      }
      case '/' :
      {
        if (!op2)                       // division by zero
        {
          fb1638_prints("Err     ", 7);
          tm1638_showbuffer();
          op1= 0; op2= 0; op1read= 1;   // Operande loeschen und flag fuer ersten Operanten lesen
          do
          {
            k= tm1638_readkeys();
            k= pgm_read_byte(&calckeymap[k-1]);
          } while (k != 0x18);
        }
        else
        {
          op1 /= op2;
          tm1638_setdez(op1,0,1);
        }
        break;
      }
      default : break;
    }
  }

  while(1);
}
