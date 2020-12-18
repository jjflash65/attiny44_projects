/* --------------------------------------------------------------
                        tm16_putint.c
    alternative Ausgabefunktionen, die es nicht in das Soft-
    waremodul "geschafft" haben, dieser Code hier kann u.U.
    auf einem 32-Bit Controller kleineren Code erzeugen
   -------------------------------------------------------------- */

uint8_t cursx = 0;

/* --------------------------------------------------------
                           tm16_setchar

     Gibt ein Zeichen auf der Anzeige aus und erhoeht die
     Cursorpostion um eins.

     Uebergabe:
        ch    : Auszugebendes Zeichen, erlaubt sind 0..9
                " ", "-"
        dpanz : auf dem aktuellen Digit wird der Dezimal-
                punkt angezeigt
   -------------------------------------------------------- */

void tm16_setchar(uint8_t ch, uint8_t dpanz)
{
  if (dpanz) dpanz= 0x80;
  if (ch== ' ')
    tm16_setbmp(cursx, dpanz);
  else
    if (ch== '-')
    tm16_setbmp(cursx, 0x40 | dpanz);
  else tm16_setbmp(cursx, led7sbmp[ch] | dpanz);
  cursx++;
}

/* ------------------------------------------------------------
                         tm16_putint_6digit
     gibt einen Integer dezimal aus. Ist Uebergabe
     "komma" != 0 wird ein "Kommapunkt" mit ausgegeben.
     Darstellbarer Zahlenbereich: -99999 bis 999999
   ------------------------------------------------------------ */

void tm16_putint_6digit(int32_t i, char komma)
{
  typedef enum boolean { FALSE, TRUE }bool_t;

  static uint32_t zz[]  = { 100000, 10000, 1000, 100, 10 };
  bool_t not_first = FALSE;
  bool_t negflag   = FALSE;

  uint8_t zi;
  uint8_t outchar;
  uint32_t z, b;

  cursx= 0;
  komma++;

  if (!i)
  {
    tm16_setchar(0,0);
  }
  else
  {
    if(i < 0)
    {
      i = -i;
      negflag= TRUE;
    }

    for(zi = 0; zi < 5; zi++)
    {
      z = 0;
      b = 0;

      while(z + zz[zi] <= i)
      {
        b++;
        z += zz[zi];
      }

      {
        if ((!b) && (!not_first)) outchar= ' ';
                             else outchar= b;
        if ((6-zi)== komma)
          tm16_setchar(outchar,1);
        else
          tm16_setchar(outchar,0);

        if (b) not_first = TRUE;
      }

      i -= z;
    }
    tm16_setchar(i,0);
    if (negflag)
    {
      cursx= 0;
      tm16_setchar('-',0);
    }
  }
}
