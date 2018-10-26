// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse CGA_Stream
 */

/*! \defgroup io I/O Subsystem
	\brief Das Eingabe-/Ausgabe Subsystem

*/

#pragma once

#include "object/o_stream.h"
#include "machine/cgascr.h"

/*! \brief Darstellung verschiedener Datentypen auf dem Bildschrim
 *  \ingroup io
 *
 *
 *  Die Klasse CGA_Stream ermöglicht die Ausgabe verschiedener Datentypen als
 *  Zeichenketten auf dem CGA Bildschirm eines PCs. Dazu braucht CGA_Stream nur
 *  von den Klassen O_Stream und CGA_Screen abgeleitet und endlich die Methode
 *  flush() implementiert werden. Für weitergehende Formatierung oder spezielle
 *  Effekte stehen die Methoden der Klasse CGA_Screen zur Verfügung.
 */
class CGA_Stream
	//TODO: Hier muss die Vererbungshierarchie vervollständigt werden.
{
	// Verhindere Kopien und Zuweisungen
	CGA_Stream(const CGA_Stream&)            = delete;
	CGA_Stream& operator=(const CGA_Stream&) = delete;
public:
	/// \copydoc CGA_Screen::CGA_Screen(int, int, int, int, bool)
	CGA_Stream(int from_col, int to_col, int from_row, int to_row,
			   bool use_cursor = false)
	//TODO: Hier muss noch Code vervollständigt werden.
	{
		(void) from_col;
		(void) to_col;
		(void) from_row;
		(void) to_row;
		(void) use_cursor;
	}

	/*! \brief Methode zur Ausgabe des Pufferinhalts der Basisklasse Stringbuffer.
	 *
	 *  Die Methode wird implizit aufgerufen, sobald der Puffer voll ist,
	 *  kann aber auch explizit verwendet werden, um eine Ausgabe zu erzwingen.
	 *
	 *  \todo Methode implementieren
	 */
	virtual void flush();
};

