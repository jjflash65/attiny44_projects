/* ----------------------------------------------------------
                           rtc_i2c.c

     Header fuer Softwaremodul zu I2C Realtime clock Bau-
     steinen DS1307 und DS3231 (gleiche Steuercodes).

     Ansprechen des Bausteins erfolgt ueber Software I2C


     MCU   : ATtiny44
     F_CPU : 8 MHz intern

     Fuses : fuer 8 MHz intern
             lo 0xe2
             hi 0xdf

     Fuses : fuer ext. Quarz >= 8 MHz
             lo 0xde
             hi 0xdf


     Pinbelegung I2C
     ---------------

     PB0 = SDA
     PB1 = SCL

     02.01.2019

   ---------------------------------------------------------- */


#include "rtc_i2c.h"

/* --------------------------------------------------
     rtc_read

     liest einen einzelnen Wert aus dem RTC-Chip

     Uebergabe:
         addr : Registeradresse des DS1307 der
                gelesen werden soll
   -------------------------------------------------- */
uint8_t rtc_read(uint8_t addr)
{
  uint8_t value;

  i2c_sendstart();
  i2c_write(rtc_addr);
  i2c_write(addr);
  i2c_stop();
  i2c_sendstart();
  i2c_write(rtc_addr | 1);
  value= i2c_read_nack();
  i2c_stop();

  return value;
}

/* --------------------------------------------------
     rtc_write

     schreibt einen einzelnen Wert aus dem RTC-Chip

     Uebergabe:
         addr : Registeradresse des DS1307 der
                geschrieben werden soll
   -------------------------------------------------- */
void rtc_write(uint8_t addr, uint8_t value)
{
  i2c_sendstart();
  i2c_write(rtc_addr);
  i2c_write(addr);
  i2c_write(value);
  i2c_stop();
}

/* --------------------------------------------------
      rtc_bcd2dez

      wandelt eine BCD Zahl (NICHT hex)  in einen
      dezimalen Wert um

      Bsp: value = 0x34
      Rueckgabe    34
   -------------------------------------------------- */
uint8_t rtc_bcd2dez(uint8_t value)
{
  uint8_t hiz,c;

  hiz= value / 16;
  c= (hiz*10)+(value & 0x0f);
  return c;
}

/* --------------------------------------------------
      rtc_getwtag

      Berechnet zu einem bestimmten Datum den
      Wochentag (nach Carl Friedrich Gauss). Der
      Function werden die Daten mittels der
      Struktur my_datum so uebergeben, wie sie
      von rtc_readdate gelesen werden. Ein Wochen-
      tag beginnt mit 0 (0 entspricht Sonntag)

      Uebergabe:
           *date  : Zeiger auf eine Struktur
                    my_datum

      Rueckgabe:
           Tag der Woche

      Bsp.:      11.04.2017   ( das ist ein Dienstag )
      Rueckgabe: 2
   -------------------------------------------------- */
uint8_t rtc_getwtag(struct my_datum *date)
{
  int tag, monat, jahr;
  int w_tag;

  tag=  rtc_bcd2dez(date->tag);
  monat= rtc_bcd2dez(date->monat);
  jahr= rtc_bcd2dez(date->jahr)+2000;

  if (monat < 3)
  {
     monat = monat + 12;
     jahr--;
  }
  w_tag = (tag+2*monat + (3*monat+3)/5 + jahr + jahr/4 - jahr/100 + jahr/400 + 1) % 7 ;
  return w_tag;
}

/* --------------------------------------------------
      rtc_readdate

      liest den DS1307 Baustein in eine Struktur
      my_datum ein.

      Rueckgabe:
          Werte der gelesenen RTC in der Struktur
          my_datum
   -------------------------------------------------- */
struct my_datum rtc_readdate(void)
{
  struct my_datum date;

  date.sek= rtc_read(0) & 0x7f;
  date.min= rtc_read(1) & 0x7f;
  date.std= rtc_read(2) & 0x3f;
  date.tag= rtc_read(4) & 0x3f;
  date.monat= rtc_read(5) & 0x1f;
  date.jahr= rtc_read(6);
  date.dow= rtc_getwtag(&date);

  return date;
}

/* --------------------------------------------------
     rtc_writedate

     schreibt die in der Struktur enthaltenen Daten
     in den RTC-Chip
   -------------------------------------------------- */
void rtc_writedate(struct my_datum *date)
{
  rtc_write(0, date->sek);
  rtc_write(1, date->min);
  rtc_write(2, date->std);
  rtc_write(4, date->tag);
  rtc_write(5, date->monat);
  rtc_write(6, date->jahr);
}

