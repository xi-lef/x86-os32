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
class CGA_Stream : public O_Stream, public CGA_Screen {
	// Verhindere Kopien und Zuweisungen
	CGA_Stream(const CGA_Stream&)            = delete;
	CGA_Stream& operator=(const CGA_Stream&) = delete;
public:
	/// \copydoc CGA_Screen::CGA_Screen(int, int, int, int, bool)
	CGA_Stream(int from_col, int to_col, int from_row, int to_row,
			   bool use_cursor = false) : CGA_Screen(from_col, to_col,
               from_row, to_row, use_cursor) {}

	/*! \brief Methode zur Ausgabe des Pufferinhalts der Basisklasse Stringbuffer.
	 *
	 *  Die Methode wird implizit aufgerufen, sobald der Puffer voll ist,
	 *  kann aber auch explizit verwendet werden, um eine Ausgabe zu erzwingen.
	 *
	 *  \todo Methode implementieren
	 */
	virtual void flush();
};

