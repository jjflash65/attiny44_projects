Softwaremodul ws2812.c / ws2812_output.S
Header        ws2812.h / ws2812_pins.h
____________________________________________________________________________________________

10.11.2018 by R. Seelig


Inhaltsverzeichnis:

        - Vorwort
        - WS2812b Leuchtdioden
        - Make
        - WS2812 Header
          . ws2812.h
          . ws2812_pins.h

        - Funktionen von WS2812

          . rgbfromega
          . rgbfromvalue
          . ws_showbuffer
          . ws_setrgbcol
          . ws_reset
          . ws_init
          . ws_clrarray
          . ws_blendup_left
          . ws_blendup_right
          . ws_buffer_rl
          . ws_buffer_rr

        - Benutzung des Softwaremoduls

_______________________________________________

Vorwort
_______________________________________________

Seit geraumer Zeit schon gibt es Leuchtdioden mit "integriertem seriellen Controller". Diese
weerden oft fuer sehr kleines Geld (und absolut haeufig auch aus China auf EBAY) angeboten.

Leider ist die Unterstuetzung fuer AVR Controller haeufig nur fuer Arduino und der MEGA
Serie zu finden, fuer einfaches C ist nicht so viel vorhanden und fuer die TINY Serie
ist es noch etwas weniger.

Aus diesen Ueberlegungen heraus ist ein Softwaremodul fuer ATtiny44 entstanden, der mit
lediglich 8MHz getaktet werden darf (ansonsten stimmt das Timing fuer die WS2812 LEDs
nicht mehr).

Vereinfacht wurde dieses Softwaremodul, nachdem ich auf der Seite:

     https://www.embeddedrelated.com/showarticle/528.php

von Mike Silva fuendig wurde (an dieser Stelle einen herzlichen Dank an ihn, ohne ihn
zu kennen).

Die vorliegende Software ist ein "Makefile Projekt", was heisst, dass es - bei einem
installierten AVR-GCC - lediglich eines Texteditors bedarf um daran Aenderungen vorzu-
nehmen.

Die Verwendung des Softwaremoduls kann somit mit:

Compiler         :  avr-gcc (ab Version 4.8.1 getestet)
MCU              :  ATtiny44 (sollte jedoch auch mit ATtiny24 und ATtiny84 funktionieren)
Taktfrequenz MCU :  8MHz intern
Fuses            :  Lo:0xE2    Hi:0xDF
LEDs             :  WS2812b

Betriebssystem   :  Linux
IDE              :  Make

betrieben werden.
_______________________________________________

WS2812b Leuchtdioden
_______________________________________________

WS2812b LEDs sind Vollfarb-LEDs (gruen / rot / blau) und besitzen einen "integrierten
seriellen Controller". Deren Funktionsweise ist in der Theorie relativ einfach (das
Programmieren hierfuer jedoch ist etwas kniffeliger).

Mittels einer einzigen Datenleitung empfaengt innerhalb einer Leuchtdiodenreihe die erste
LED Informationen. Diese sind 3 Bytes, jeweils eines fuer den Farbanteil von gruen, rot
und blau. Treffen nach diesen 3 Bytes weitere Bytes ein, werden die empfangenen Bytes
an den Ausgang der LED geschoben und einer weiteren nachgeschalteten LED uebergeben.

Reist der Datenstrom fuer eine gewisse Zeit ab (in der Regel betraegt diese Zeit
50 uSekunden) wird ein neuer Datenframe als Frame fuer die erste LED gewertet.

Eine WS2812b Leuchtdiode hat somit 4 Anschluesse: +Vcc, GND, Data_in und Data_out.

Beispiel: LED-Strang mit 4 LEDs

       +Vcc
       o------------+--------------+---------------+--------------+---------
                    |Vcc           |Vcc            |Vcc           |Vcc
                    |              |               |              |
       Data    +----+----+    +----+----+     +----+----+    +----+----+
       o-------| in  out |----| in  out |-----| in  out |----| in  out |----
               +----+----+    +----+----+     +----+----+    +----+----+
                    |              |               |              |
       GND       GND|           GND|            GND|           GND|
       o------------+--------------+---------------+--------------+---------

In welcher Form liegen die Daten fuer WS2812 Leuchtdioden vor?

Es ist ein relativ einfaches Protokoll, welches jedoch relativ sehr zeitkritisch ist.
Fuer ein Byte muessen 8 Bits uebertragen werden. Nachdem am Data-Input fuer min.
50 uSekunden ein Lo-Signal angelegt wird, beginnt die Datenuebertragung mit der ersten
Lo-Hi Flanke.

Ein Datenbit wird innerhalb einer Zeitspanne von 1,25 uSekunden uebertragen. Betraegt
die Pulsdauer des Signals 350 nSekunden (n= nano !!!) und 900 nSekunden die Pausedauer,
so wertet die WS2812 LED dieses als eingehende 0.

Betraegt die Pulsdauer 900nS und die Pausedauer 350nS, wird dieses als eingehende 1
gewertet.

Die Informationen werden in die LED in der Folge: gruen, rot und blau und die einzelnen
Bits mit der hoeheren Wertigkeit zuerst uebertragen. 3 Byte zu je 8 Bit benoetigen
somit 24 Impulse (mit dem oben beschriebenen Timing).

Dieses Timing mit einem ATtiny einzuhalten ist nicht ganz einfach und aus diesem Grund
besteht eine Datei dieses Softwaremoduls aus einer Assemblerdatei.

Das vorliegende ZIP-Archiv, zu dem diese Readme Datei gehoert, enthaellt alle Dateien,
die zum einfachen Uebersetzen des Demoprogrammes notwendig sind (ein installierter
AVR-GCC Compiler wird vorausgesetzt).

_______________________________________________

Make
_______________________________________________

Bei der Programmierung hat es sich mehr oder weniger durchgesetzt, eine Uebersetzung
von Quellcodes und eines Linken der uebersetzten Dateien, einer sogenannten Make Datei
zu ueberlassen. Auf Betriebssystemebene wird auf einer Kommandozeile

                                      make

in dem Ordner aufgerufen, in dem sich das Quellprogramm befindet. Selbst bei Be-
nutzung einer IDE wird dieses einem Make ueberlassen, welches jedoch haeufig in die
IDE integriert ist und die Makedatei aus Projekteinstelldaten erzeugt wird.

Uebersichtlicher jedoch kann es sein (Meinung des Autors), wenn die Makedatei in einer
Textdatei vorliegt (eben eine Datei mit dem Namen "Makefile"), um entsprechend direkt
mittels Angaben die Programmgenerierung steuern zu koennen.

Zu diesem Zwecke wurde hier Make in 2 Dateien aufgeteilt (case sensitiv):

      - Makefile
      - makefile.mk

Das Makefile ist sehr einfach gestaltet und an sich selbsterklaerend. Hier koennen
Angaben gemacht werden, welche Programmmodule zum Hauptprogramm hinzugefuegt werden
sollen, sowie die Pfade, in denen diese Module liegen koennen.

Im Vorliegenden Falle gibt es fuer einen Upload in die MCU mehrere Moeglichkeiten:

   PROGRAMMER = "Programmername"

Als "Programmername" kann angegeben werden (je nach dem was verwendet wird):
   usbasp
   usbtiny
   avrisp       (das waere der Programmer aus der Beispielsdatei eines
                 Arduino)
   stk500v2
   ponyser

Fuer die Typen avrisp und stk500v2 muessen der Programmerport und die Baudrate an-
gegeben werden.

Beispiel fuer STK500V2:
  PROGRAMMER = stk500v2
  PROGPORT   = /dev/ttyACM0
  BRATE      = 115200
  DUDEOPTS   = -B1

Beispiel fuer AVRISP (generiert aus Chinaclone Arduino):
  PROGRAMMER = avrisp
  PROGPORT   = /dev/ttyUSB0
  BRATE      = 19200
  DUDEOPTS   = -B1


Weitere Sourcesoftware kann mittels

  SRCS   +=  nocheinedatei.p

hinzugefuegt werden. Zu beachten, dass es hierfuer eine Datei Namens nocheinedatei.c
oder nocheinedatei.S in den angegebenen Suchpfaden geben muss. Diese wird zu
nocheinedatei.o uebersetzt.

Makefile inkludiert eine weitere Datei: makefile.mk. Diese wertet die Benutzer-
angaben aus und generiert das Programm im Intel Hex-Format. Innerhalb dieser inkludierten
Datei werden Quelltexte uebersetzt und zu einem Gesamtprogramm zusammengelinkt.

_______________________________________________

WS2812 Header
_______________________________________________


    ws2812.h
    --------

      In ws2812.h sind Strukturen, Farbvorbelegungen (mit den Farbwerten der
      alten EGA Grafikkarte) sowie die Funktionsprototypen enthalten.

      Strukturen, Variable und Funktionen von WS2812
      -----------------------------------------------

        Das Array -  egapalette - beinhaltet 2 x 16 Farben die den "alten" Farben einer
        EGA Grafikkarte (und den 16 Standardfarben unter Windows) entsprechen. Jede Farbe
        benoetigt 3 Bytes (je eines fuer rot, gruen und blau). Die unteren 16 Farben sind
        die Farben in "dunkel", die oberen 16 Farben sind dieselben Farben mit voller
        Leuchtstaerke

    ws2812_pins.h
    -------------

       In dieser Datei wird angegeben, an welchen Pin des Controllers die Datenleitung
       der WS2812 Leuchtdiodenkette anzuschliesen ist (von diesem Pin ist ausserdem ein
       Pop-Up Widerstand von 2,2k nach Vcc zu schalten).


         #define ws_port      PORTB
         #define ws_ddr       DDRB
         #define ws_portpin   1

       deklariert PB1 als Anschlusspin fuer die Leuchtdiodenkette.


Funktionen von WS2812
________________________________________________________________________________________


     rgbfromega
     --------------------------------------------

     void rgbfromega(uint8_t eganr, struct colvalue *f);


     beschreibt eine struct Variable auf die *f zeigt mit einem Farbwert, der aus der
     EGA-Palette entnommen wird

     Uebergabe:
                  nr   : Farbeintrag der EGA-Palette
                  *f   : Zeiger auf die zu beschreibende
                         struct Variable

     rgbfromvalue
     --------------------------------------------

     void rgbfromvalue(uint8_t r, uint8_t g, uint8_t b, struct colvalue *f);


     beschreibt eine struct Variable auf die *f zeigt mit einem RGB Farbwert.

     Uebergabe:
                r,g,b   : RGB - Farbwert
                   *f   : Zeiger auf die zu beschreibende
                          struct Variable

     ws_showbuffer
     --------------------------------------------

     void ws_showbuffer(uint8_t *ptr, int anz)

     Sendet einen Datenstream aus dem Puffer, der mit dem Zeiger ptr adressiert
     ist, and die Leuchtdiodenkette (hierdurch, und nur hierdurch, wird der
     Puffer auf der Leuchtdiodenkette angezeigt).

     Uebergabe:
               *ptr  : Zeiger auf ein Array, das angezeigt
                       werden soll
               anz   : Anzahl der anzuzeigenden LED's



     ws_setrgbcol
     --------------------------------------------

     void ws_setrgbcol(uint8_t *ptr, uint16_t nr, struct colvalue *f)

     setzt den Farbwert der in

                   struct colvalue f

     angegeben ist, in ein Array an Stelle nr ein. Dieses Array kann mittels
     ws_showbuffer auf der LED-Kette ausgegeben werden kann.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                     nr : LED-Nummer im Array, welche die
                          Position im Leuchtdiodenstrang
                          repraesentiert
     struct colvalue *f : Zeiger auf einen RGB-Farbwert


     ws_reset
     --------------------------------------------

     void ws_reset(void);

     setzt die Leuchtdiodenreihe (fuer einen ncahfolgenden Datentransfer
     zu den Leuchtdioden) zurueck


     ws_init
     --------------------------------------------

     void ws_init(void);

     initialisiert den in ws2812_pins.h angegebenen Anschluss als Ausgang fuer die
     LED Kette und setzt die Kette zurueck.


     ws_clrarray
     --------------------------------------------

     void ws_clrarray(uint8_t *ptr, int anz)

     loescht ein LED Anzeigearray

     Parameter:
                   *ptr : Zeiger auf ein Array, das
                          geloescht werden soll
                    anz : Anzahl der LEDs, die angezeigt
                          werden sollen


     ws_blendup_left
     --------------------------------------------

     void ws_blendup_left(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime)


     blendet eine LED-Anzahl anz mit dem Farbwert, der in

                     struct colvalue f

     angegeben ist, links schiebend auf. Hierfuer wird das Array, auf das *ptr
     zeigt mit dem Farbwert aufgefuellt.

     Verzoegerungszeit dtime bestimmt die Dauer eines Einzelschrittes beim Aufbau in
     ca. dtime * 1.6ms

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                    anz : Anzahl der Leuchtdioden im Strang
                    *f  : Zeiger auf RGB-Farbwertstruktur die die Farbe der Auf-
                          blendung enthaellt
                  dtime : Verzoegerungszeit eines Schrittes beim Aufblenden


     ws_blendup_right
     --------------------------------------------

     void ws_blendup_right(uint8_t *ptr, uint8_t anz, struct colvalue *f, int dtime)

     blendet eine LED-Anzahl anz mit dem Farbwert, der in


                      struct colvalue f

     angegeben ist, rechts schiebend auf. Hierfuer wird das Array, auf das *ptr
     zeigt mit dem Farbwert aufgefuellt.

     Verzoegerungszeit dtime bestimmt die Dauer eines Einzelschrittes beim Aufbau in
     ca. dtime * 1.6ms

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                    anz : Anzahl der Leuchtdioden im Strang
                    *f  : Zeiger auf RGB-Farbwertstruktur die die Farbe der Auf-
                          blendung enthaellt
                  dtime : Verzoegerungszeit eines Schrittes beim Aufblenden


     ws_buffer_rl
     --------------------------------------------

     void ws_buffer_rl(uint8_t *ptr, uint8_t lanz)


     rotiert einen Pufferspeicher der die Leuchtdiodenmatrix enthaelt um eine LED-Position
     nach links (also 3 Bytes) und fuegt hierbei die am Ende anstehende LED am Anfang
     wieder ein.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                   lanz : Anzahl der Leuchtdioden im Strang


     ws_buffer_rr
     --------------------------------------------

     void ws_buffer_rl(uint8_t *ptr, uint8_t lanz)


     rotiert einen Pufferspeicher der die Leuchtdiodenmatrix enthaelt um eine LED-Position
     nach rechts (also 3 Bytes) und fuegt hierbei die am Anfang anstehende LED am Ende
     wieder ein.

     Parameter:
                   *ptr : Zeiger auf ein Array, das die
                          RGB Farbwerte aufnehmen soll
                   lanz : Anzahl der Leuchtdioden im Strang


_______________________________________________

Benutzung des Softwaremoduls
_______________________________________________


Die grundsaetzliche Vorgehensweise, um mit diesem Softwaremoduls WS2812 Leuchtdioden zu be-
schreiben ist:

   - Farbwert mischen (oder aus EGA-Palette) holen und diese in einer Variable
     vom Type "struct colvalue" zu speichern.

   - den Farbwert in einem Pufferspeicher ablegen.

   - den Pufferspeicher anzeigen

   Beispiel:

     // Farbwert lila an Position 4 auf der LED-Kette einfuegen und anzeigen

         #define ledanz     12
         struct colvalue    rgbcol;
         uint8_t            ledbuffer[ledanz * 3];

         ...
         rgbfromvalue(255,0,255 &rgbcol);                 // Farbe mischen
         ws_setrgbcol(&ledbuffer[0], 4, &rgbcol);         // in Pufferarray einfuegen
         ws_showbuffer(&ledbuffer[0], ledanz);            // Puffer anzeigen


   Siehe auch Demoprogramm "ws2812_demo.c"


________________________________________________________________________________________________________________

Text und Software von R. Seelig

Jegliche Verwendung der Software und des Textes ist ausdruecklich erlaubt. Kommerzielle Nutzung der Software
inklusive dieses Textes insbesondere in gedruckter Form bedarf der Zustimmung des Authors R. Seelig
