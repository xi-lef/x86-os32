// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse ConsoleOut
 */

#pragma once

#include "o_stream.h"

/*! \brief Ausgabe auf der Konsole
 *
 *  Die Klasse ConsoleOut ermöglicht ein Schreiben auf der Konsole ähnlich std::cout
 *  aus der C++-Standardsbibliothek. Die Klasse ist von O_Stream abgeleitet.
 */
class ConsoleOut : public O_Stream
{
	// Verhindere Kopien und Zuweisungen
	ConsoleOut(const ConsoleOut&)            = delete;
	ConsoleOut& operator=(const ConsoleOut&) = delete;
public:
	/*! \brief Konstruktor
	 *
	 */
	ConsoleOut();

	/*! \brief Ausgabe der Zeichenkette auf dem Bildschirm.
	 *  Die Implementierung soll ausschliesslich `putchar()` verwenden.
	 */
	virtual void flush();
};

