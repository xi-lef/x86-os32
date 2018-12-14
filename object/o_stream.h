// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Hier ist die Klasse O_Stream implementiert.
 *  Neben der Klasse O_Stream sind hier auch die Manipulatoren \c hex , \c dec ,
 *  \c oct und \c bin für die Wahl der Basis bei der Zahlendarstellung,
 *  sowie \c endl für den Zeilenumbruch \b deklariert.
 *  \ingroup io
 *
 *
 *  \par Manipulatoren
 *  Um bei der Textformatierung mit Hilfe der Klasse O_Stream das Zahlensystem
 *  bequem wählen und Zeilenumbrüche einfügen zu können, sollen sogenannte
 *  Manipulatoren definiert werden.
 *  Der Ausdruck <tt>kout << "a = " << dec << a << " ist hexadezimal " << hex << a << endl;</tt>
 *  soll dann beispielsweise den Wert der Variablen a erst in dezimaler und
 *  dann in hexadezimaler Schreibweise formatieren und zum Schluss einen
 *  Zeilenumbruch anfügen.
 *
 *  Die gewünschten Eigenschaften können realisiert werden, wenn hex, dec,
 *  oct, bin und endl als Funktionen (d.h. nicht als Methoden der Klasse
 *  O_Stream) definiert werden, die als Parameter und Rückgabewert jeweils
 *  eine Referenz auf ein O_Stream Objekt erhalten bzw. liefern. Durch diese
 *  Signatur wird bei dem genannten Ausdruck der bereits erwähnte Operator
 *  <tt>O_Stream& O_Stream::operator<< ((*fkt*) (O_Stream&))</tt> ausgewählt, der dann
 *  nur noch die als Parameter angegebene Funktion ausführen muss.
 *
 *  \par Anmerkung
 *  Der Manipulatorbegriff wurde dem Buch 
 *  <a href="http://www.stroustrup.com/4th.html">The C++ Programming Language</a>
 *  von Bjarne Stroustrup entnommen.
 *  Dort finden sich auch weitergehende Erläuterungen dazu.
 */

#pragma once

#include "strbuf.h"
#include "types.h"
#include "user/time/time.h"

/*! \brief Die Aufgaben der Klasse O_Stream entsprechen im Wesentlichen denen der
 *  Klasse ostream der bekannten C++ IO-Streams-Bibliothek.
 *
 *  Da die Methode \c Stringbuffer::put(char) der Basisklasse Stringbuffer recht
 *  unbequem ist, wenn die zusammenzustellenden Texte nicht nur aus einzelnen
 *  Zeichen, sondern auch aus Zahlen, oder selbst wieder aus Zeichenketten
 *  bestehen, werden in der Klasse O_Stream Möglichkeiten zum Zusammenstellen
 *  verschiedener Datentypen realisiert. In Anlehnung an die bekannten
 *  Ausgabeoperatoren der C++ IO-Streams-Bibliothek wird dazu der
 *  Shift-Operator \c operator<< verwendet.
 *
 *  Darüberhinaus soll es möglich sein, für die Darstellung ganzer Zahlen
 *  zwischen dem Dezimal-, dem Binär- dem Oktal- und dem Hexadezimalsystem zu
 *  wählen. Beachtet dabei bitte die übliche Darstellung negativer Zahlen: Im
 *  Dezimalsystem mit führendem Minuszeichen, im Oktal- und Hexadezimalsystem
 *  ohne Minuszeichen, sondern genau so wie sie im Maschinenwort stehen.
 *  (Intel-CPUs verwenden intern das 2er-Komplement für negative Zahlen. \c -1
 *  ist Hexadeziamal also \c FFFFFFFF und Oktal \c 37777777777.)
 *
 *  Die öffentlichen Methoden/Operatoren von O_Stream liefern jeweils eine
 *  Referenz auf ihr eigenes O_Stream Objekt zurück. Dadurch ist es möglich,
 *  in einem Ausdruck mehrere der Operatoren zu verwenden, z. B.
 *  <tt>kout << "a = " << a</tt>;
 *
 *  Zur Zeit wird die Darstellung von Zeichen, Zeichenketten und ganzen Zahlen
 *  unterstützt. Ein weiterer \c << Operator erlaubt die Verwendung von
 *  Manipulatoren, welche in der o_stream.h-Dokumentation näher
 *  beschrieben sind.
 */

class O_Stream : public Stringbuffer {
	O_Stream(const O_Stream&)            = delete;
	O_Stream& operator=(const O_Stream&) = delete;

public:
	/*! \brief Basis des zur Anzeige verwendeten Zahlensystems (z.B. 2, 8, 10 oder 16)
	 *
	 */
	int base;

	/*! \brief Konstruktor; Initale Zahlenbasis ist das Dezimalsystem.
	 *
	 */
	O_Stream() {
		base = 10;
	}

	/*! \brief Destruktor
	 */
	virtual ~O_Stream () {}

	/*! \brief Leert den Puffer.
	 *
	 *  Rein virtuelle Methode, die erst durch abgeleitete Klassen implementiert
	 *  wird. Darstellung des Pufferinhalts kann so durch unterschiedliche Kindklassen
	 *  variiert werden.
	 */
	virtual void flush () = 0;

	/*! \brief Stellt ein einzelnes Zeichen dar
	 *
	 *  \todo Operator implementieren
	 *
	 *  \param c Darzustellendes Zeichen
	 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	O_Stream& operator << (char c);
    //
	/// \copydoc O_Stream::operator<<(char)
	O_Stream& operator << (unsigned char c);

	/*! \brief Darstellung einer nullterminierten Zeichenkette
	 *
	 *  \todo Operator implementieren
	 *
	 *  \param string Darzustellende Zeichenkette.
	 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	O_Stream& operator << (const char* string);

	/*! \brief Stellt ein Boolean dar
	 *
	 *  \todo Operator implementieren
	 *
	 *  \param b Darzustellendes Boolean
	 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	O_Stream& operator << (bool b);

	/*! \brief Darstellung ganzer Zahlen im Zahlensystem zur Basis base
	 *
	 *  \todo Operator implementieren
	 *
	 *  \param ival Darzustellende Zahl
	 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	O_Stream& operator << (short ival);

	/// \copydoc O_Stream::operator<<(short)
	O_Stream& operator << (unsigned short ival);

	/// \copydoc O_Stream::operator<<(short)
	O_Stream& operator << (int ival);

	/// \copydoc O_Stream::operator<<(short)
	O_Stream& operator << (unsigned int ival);

	/// \copydoc O_Stream::operator<<(short)
	O_Stream& operator << (long ival);

	/// \copydoc O_Stream::operator<<(short)
	O_Stream& operator << (unsigned long ival);

	/*! \brief Darstellung eines Zeigers als hexadezimale ganze Zahl
	 *
	 *  \todo Operator implementieren
	 *
	 *  \param ptr Darzustellender Pointer
	 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	O_Stream& operator << (const void* ptr);

	/*! \brief Aufruf einer Manipulatorfunktion
	 *
	 *  Methode sorgt dafür, dass Manipulatorfunktionen aufgerufen werden,
	 *  um Veränderungen an der Klasse (z.B. Änderung der Zahlenbasis) zu
	 *  ermöglichen.
	 *
	 *  \todo Operator implementieren
	 *
	 *  \param f Anzuwendende Manipulatorfunktion
	 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	O_Stream& operator << (O_Stream& (*f) (O_Stream&));

#define TIME_DISPLAY_LENGTH 25

    /*!
     * Darstellung der Uhrzeit und des Datums.
     * Beansprucht TIME_DISPLAY_LENGTH viele Zeichen.
     */
    O_Stream& operator << (Time t);

private:
    /*!
     * Hilfsmethoden für die Ausgabe.
     */
    void put_signed(long ival);

    void put_unsigned(unsigned long ival);
};

/*! \brief Löst explizit ein Leeren (Flush) des Puffers aus.
 *
 *  \todo Modifikator implementieren
 *
 *  \param os Zu modifizierender Stream
 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
 */
O_Stream& flush(O_Stream& os);

/*! \brief Fügt einen Zeilenumbruch in die Ausgabe ein und löst ein Leeren (Flush) des Puffers aus.
 *
 *  \todo Modifikator implementieren
 *
 *  \param os Zu modifizierender Stream
 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
 */
O_Stream& endl(O_Stream& os);

/*! \brief Wählt das binäre Zahlensystem aus.
 *
 *  \todo Modifikator implementieren
 *
 *  \param os Zu modifizierender Stream
 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
 */
O_Stream& bin(O_Stream& os);

/*! \brief Wählt das oktale Zahlensystem aus.
 *
 *  \todo Modifikator implementieren
 *
 *  \param os Zu modifizierender Stream
 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
 */
O_Stream& oct(O_Stream& os);

/*! \brief Wählt das dezimale Zahlensystem aus.
 *
 *  \todo Modifikator implementieren
 *
 *  \param os Zu modifizierender Stream
 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
 */
O_Stream& dec(O_Stream& os);

/*! \brief Wählt das hexadezimale Zahlensystem aus.
 *
 *  \todo Modifikator implementieren
 *
 *  \param os Zu modifizierender Stream
 *  \return Referenz auf ein O_Stream Objekt, um Operatoren konkatenieren zu können.
 */
O_Stream& hex(O_Stream& os);

