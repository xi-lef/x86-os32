// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Panic
 */

#pragma once

/* INCLUDES */

#include "guard/gate.h"

/*! \brief Standardunterbrechungsbehandlung
 *
 *  Die Klasse Panic dient der Behandlung von Unterbrechungen und Ausnahmen.
 *  Nach der Ausgabe einer Fehlermeldung wird der Prozessor angehalten. Bei der
 *  Initialisierung der Plugbox wird diese Form der Unterbrechungsbehandlung
 *  für alle Interrupt-Nummern eingetragen.
 */
class Panic
	: public Gate
{
	// Verhindere Kopien und Zuweisungen
	Panic(const Panic&)            = delete;
	Panic& operator=(const Panic&) = delete;
public:
	Panic() {}
	/*! \brief Einfachste Form der Unterbrechungsbehandlung: Es wird eine
	 *  Fehlermeldung ausgegeben und der Prozessor angehalten.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void trigger();
};

