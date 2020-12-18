/* -----------------------------------------------------
                        rda5807.c

    Softwaremodul fuer ein RDA5807 UKW- Empfaengers
    (I2C Interface)

     MCU      : ATtiny44
     F_CPU    : 8 MHz intern

     Hardware : RDA5807 IC

     Fuses    : fuer 8 MHz intern
                 lo 0xe2
                 hi 0xdf


     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL


     28.09.2018 R. Seelig

  ------------------------------------------------------ */

#include "rda5807.h"

uint16_t aktfreq =   1018;      // Startfrequenz ( 101.8 MHz )
uint8_t  aktvol  =   2;         // Startlautstaerke

uint16_t festfreq[6] = { 1018, 1048, 888, 970, 978, 999 };

uint8_t  rda5807_adrs = 0x10;   // I2C-addr. fuer sequientielllen Zugriff
uint8_t  rda5807_adrr = 0x11;   // I2C-addr. fuer wahlfreien Zugriff
uint8_t  rda5807_adrt = 0x60;   // I2C-addr. fuer TEA5767 kompatiblen Modus

uint16_t rda5807_regdef[10] ={
            0x0758,             // 00 default ID
            0x0000,             // 01 reserved
            0xF009,             // 02 DHIZ, DMUTE, BASS, POWERUPENABLE, RDS
            0x0000,             // 03 reserved
            0x1400,             // 04 SOFTMUTE
            0x84DF,             // 05 INT_MODE, SEEKTH=0110, unbekannt, Volume=15
            0x4000,             // 06 OPENMODE=01
            0x0000,             // 07 reserved
            0x0000,             // 08 reserved
            0x0000 };           // 09 reserved

uint16_t rda5807_reg[16];


/* --------------------------------------------------
      rda5807_writereg

   einzelnes Register des RDA5807 schreiben
   -------------------------------------------------- */
void rda5807_writereg(char reg)
{
  i2c_startaddr(rda5807_adrr,0);
  i2c_write(reg);                        // Registernummer schreiben
  i2c_write16(rda5807_reg[reg]);         // 16 Bit Registerinhalt schreiben
  i2c_stop();
}

/* --------------------------------------------------
      rda5807_write

   alle Register es RDA5807 schreiben
   -------------------------------------------------- */
void rda5807_write(void)
{
  uint8_t i ;

  i2c_startaddr(rda5807_adrs,0);
  for (i= 2; i< 7; i++)
  {
    i2c_write16(rda5807_reg[i]);
  }
  i2c_stop();
}

/* --------------------------------------------------
      rda5807_reset
   -------------------------------------------------- */
void rda5807_reset(void)
{
  uint8_t i;
  for (i= 0; i< 7; i++)
  {
    rda5807_reg[i]= rda5807_regdef[i];
  }
  rda5807_reg[2]= rda5807_reg[2] | 0x0002;    // Enable SoftReset
  rda5807_write();
  rda5807_reg[2]= rda5807_reg[2] & 0xFFFB;    // Disable SoftReset
}

/* --------------------------------------------------
      rda5807_poweron
   -------------------------------------------------- */
void rda5807_poweron(void)
{
  rda5807_reg[3]= rda5807_reg[3] | 0x010;   // Enable Tuning
  rda5807_reg[2]= rda5807_reg[2] | 0x001;   // Enable PowerOn

  rda5807_write();

  rda5807_reg[3]= rda5807_reg[3] & 0xFFEF;  // Disable Tuning
}

/* --------------------------------------------------
      rda5807_setfreq

      setzt angegebene Frequenz * 0.1 MHz

      Bsp.:
         rda5807_setfreq(1018);    // setzt 101.8 MHz
                                   // die neue Welle
   -------------------------------------------------- */
int rda5807_setfreq(uint16_t channel)
{

  channel -= fbandmin;
  channel&= 0x03FF;
  rda5807_reg[3]= channel * 64 + 0x10;  // Channel + TUNE-Bit + Band=00(87-108) + Space=00(100kHz)

  i2c_startaddr(rda5807_adrs,0);
  i2c_write16(0xD009);
  i2c_write16(rda5807_reg[3]);
  i2c_stop();

  delay(100);
  return 0;
}

/* --------------------------------------------------
      rda5807_setvol

      setzt Lautstaerke, zulaessige Werte fuer
      setvol 0 .. 15
   -------------------------------------------------- */
void rda5807_setvol(int setvol)
{
  rda5807_reg[5]=(rda5807_reg[5] & 0xFFF0) | setvol;
  rda5807_writereg(5);
}

/* --------------------------------------------------
      rda5807_setmono
   -------------------------------------------------- */
void rda5807_setmono(void)
{
  rda5807_reg[2]=(rda5807_reg[2] | 0x2000);
  rda5807_writereg(2);
}

/* --------------------------------------------------
      rda5807_setstero
   -------------------------------------------------- */
void rda5807_setstereo(void)
{
  rda5807_reg[2]=(rda5807_reg[2] & 0xdfff);
  rda5807_writereg(2);
}

/* --------------------------------------------------
      rda5807_getsig

     liefert die Empfangsstaerke des aktuell ein-
     gestellten Senders.
   -------------------------------------------------- */
uint8_t rda5807_getsig(void)
{
  static uint8_t b ,b2;
  static uint8_t i;

  delay(100);
  i2c_startaddr(rda5807_adrs,1);
  for (i= 0; i < 3; i++)
  {
    b= i2c_read_ack();
    delay(5);
    if (i == 2)
    {
      b2= b;
    }
  }
  b= i2c_read_nack();

  i2c_stop();
  return b2;
}

/* --------------------------------------------------
      rda5807_init

      initialisiert den Empfaengerchip (und setzt ein
      bereits initialisiertes I2C voraus)
   -------------------------------------------------- */
void rda5807_init(void)
{
  rda5807_reset();
  rda5807_poweron();
  rda5807_setmono();
  rda5807_setfreq(aktfreq);
  rda5807_setvol(aktvol);
}
