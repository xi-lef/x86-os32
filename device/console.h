// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse zur Terminalausgabe über den seriellen Anschluss
 */

#pragma once

#include "object/o_stream.h"
#include "machine/serial.h"
#include "guard/gate.h"

/*! \brief Konsole (VT100) über serielle Schnittstelle.
 *  \ingroup io
 *
 *  Mit Hilfe dieser Klasse kann ein VT100-kompatibles Anzeigeterminal über
 *  die serielle Schnittstelle verbunden werden. Mit dem Programm `screen`
 *  kann komfortabel ein Terminal unter Angabe der Schnittstelle sowie
 *  Verbindungsgeschwindigkeit geöffnet werden, z.B. `screen /dev/ttyS0 115200`
 *
 *  Durch <a href="http://ascii-table.com/ansi-escape-sequences-vt-100.php">Escape-Codes</a>
 *  kann die Darstellung (u.a. Farbe und Position) beeinflusst werden.
 */

class Console
	: public O_Stream, public Serial, public Gate
{
private:
	/*! \brief Mehrstellige Zahl als ASCII schreiben
	 */
	void write_number(int x);

public:
	/*! \brief Attribtue
	 *
	 *  Mittels Attribute kann die Darstellung der Ausgabe beeinflusst werden.
	 */
	typedef enum {
		RESET = 0,
		BRIGHT = 1,
		DIM = 2,
		UNDERSCORE = 4,
		BLINK = 5,
		REVERSE = 7,
		HIDDEN = 8,
	} attrib;

	/*! \brief Farbcodes
	 *
	 *  Für Vorder- und Hintergrund sind acht Farbattribute vorhanden.
	 *  Die tatsächliche Farbe wird von den anderen Attributen beeinflusst und
	 *  kann je nacht Terminalemulator deutlich unterschiedlich aussehen.
	 */
	typedef enum {
		BLACK = 0,
		RED = 1,
		GREEN = 2,
		YELLOW = 3,
		BLUE = 4,
		MAGENTA = 5,
		CYAN = 6,
		WHITE = 7
	} color;

	/*! \brief Konstruktor für die VT100 kompatible Konsole
	 *
	 * beinhaltet die serielle Verbindung
	 *
	 *  \opt Methode implementieren
	 */
	Console(Serial::comPort port = COM1, Serial::baudRate baudrate = BAUD_9600, Serial::dataBits databits = DATA_8BIT, Serial::stopBits stopbits = STOP_1BIT, Serial::parity parity = PARITY_NONE);

	/*! \brief Destruktor
	 */
	virtual ~Console() {}

	/*! \brief Methode zur Ausgabe des Pufferinhalts der Basisklasse Stringbuffer.
	 *
	 *  Die Methode wird implizit aufgerufen, sobald der Puffer voll ist,
	 *  kann aber auch explizit verwendet werden, um eine Ausgabe zu erzwingen.
	 *
	 * \opt Methode implementieren
	 */
	virtual void flush() override;

	/*! \brief Vordergrundfarbe (für alle nachfolgenden Ausgaben) ändern
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param c Farbe
	 */
	void setForeground(Console::color c);

	/*! \brief Hintergrundfarbe (für alle nachfolgenden Ausgaben) ändern
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param c Farbe
	 */
	void setBackground(Console::color c);

	/*! \brief Ausgabeformatierung (für alle nachfolgenden Ausgaben) ändern
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param a Ausgabettribut
	 */
	void setAttribute(Console::attrib a);

	/*! \brief Terminal zurücksetzen
	 *
	 * Bildschirm löschen, Cursor auf den Anfang setzen und Farben sowie
	 * Attribute auf den Standardwert zurücksetzen
	 *
	 *  \opt Methode implementieren
	 */
	void reset();

	/*! \brief Setzen des Cursors in Spalte \p x und Zeile \p y.
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param x Spalte
	 *  \param y Zeile
	 */
	void setpos (int x, int y);

	/*! \brief Abfragen der Cursorpostion
	 *
	 * Es ist möglich die aktuelle Cursorpostion über einen speziellen Escape-
	 * code zu empfangen. Allerdings kann es abhängig von der Implementierung
	 * sein, dass aufgrund eines nicht angeschlossenen Terminals oder
	 * Datenübertragungsfehlers endlos gewartet wird.
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param x Spalte
	 *  \param y Zeile
	 *  \return \c true , falls die Position erfolgreich empfangen wurde
	 */
	bool getpos (int &x, int &y);

	/*! \brief Anzeige mehrerer Zeichen ab der aktuellen Cursorposition
	 *
	 *  Mit dieser Methode kann eine Zeichenkette \p string ausgegeben werden, wobei
	 *  an der aktuellen Position des Cursors begonnen wird. Da die Zeichenkette
	 *  anders als sonst bei C üblich keine Nullterminierung zu enthalten braucht,
	 *  wird der Parameter \p length benötigt, der angeben muss, aus wievielen Zeichen
	 *  string besteht.
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param string Auszugebende Zeichenkette
	 *  \param length Länge der Zeichenkette
	 */
	void print (char* string, int length);

    bool prologue() override;

    void epilogue() override;

    void listen();
};

extern Console console;
