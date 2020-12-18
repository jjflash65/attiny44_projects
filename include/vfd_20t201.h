/* ----------------------------------------------------------
     vfd_20t201.h

     Header Softwaremodul fuer Vakuumfluoreszenz Display
     VFD-20T201DA2 von Samsung.

     MCU      :  Attiny44
     Takt     :  8 MHz intern

     Fuses    :  Lo:0xE2    Hi:0xDF

     04.01.2019  R. Seelig

   ---------------------------------------------------------- */

#ifndef in_vfd20t
  #define in_vfd20t

  #include <util/delay.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>

  #include "avr_gpio.h"

  /* ----------------------------------------------------------
        Anschluesse am Display:

        1 --- Vcc
        2 --- clk
        3 --- Gnd
        4 --- data
        5 --- rst
     ---------------------------------------------------------- */

  // Anschluss data: PB1
  #define vfd_datport            B
  #define vfd_datbitnr           1

  // Anschluss clk: PB0
  #define vfd_clkport            B
  #define vfd_clkbitnr           0

  // Anschluss rst: PA1
  #define vfd_rstport            A
  #define vfd_rstbitnr           1

  // ----------------------------------------------------------------
  //                         Prototypen
  // ----------------------------------------------------------------

  void vfd_init(void);
  void vfd_send(uint8_t value);
  void vfd_brightness(uint8_t value);
  void vfd_setuserchar(uint8_t nr, const uint8_t *userchar);
  void gotoxy(uint8_t x, uint8_t y);
  void clrscr(void);

  #define vfd_putchar(nr)        vfd_send(nr)

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

  #define datport           conc2(PORT,vfd_datport)
  #define datddr            conc2(DDR,vfd_datport)
  #define clkport           conc2(PORT,vfd_clkport)
  #define clkddr            conc2(DDR,vfd_clkport)
  #define rstport           conc2(PORT,vfd_rstport)
  #define rstddr            conc2(DDR,vfd_rstport)

  #define datmask           (1 << vfd_datbitnr)
  #define clkmask           (1 << vfd_clkbitnr)
  #define rstmask           (1 << vfd_rstbitnr)

  #define vfddat_init()     datddr|= datmask
  #define vfddat_set()      datport|= datmask
  #define vfddat_clr()      datport&= ~(datmask)

  #define vfdclk_init()     clkddr|= clkmask
  #define vfdclk_set()      clkport|= clkmask
  #define vfdclk_clr()      clkport&= ~(clkmask)

  #define vfdrst_init()     rstddr|= rstmask
  #define vfdrst_set()      rstport|= rstmask
  #define vfdrst_clr()      rstport&= ~(rstmask)

  #define vfdpin_init()     { vfddat_init(); vfdclk_init(); vfdrst_init(); }


#endif
