// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse Guarded_Semaphore
 */

#include "meeting/semaphore.h"
#include "guard/secure.h"

/*! \brief Systemaufrufschnittstelle zum Semaphor
 *
 *  Die Klasse Guarded_Semaphore implementiert die Systemaufrufschnittstelle zur
 *  Semaphore Klasse. Die von Guarded_Semaphore angebotenen Methoden werden
 *  direkt auf die Methoden der Basisklasse abgebildet, nur dass ihre Ausführung
 *  jeweils mit Hilfe eines Objekts der Klasse Secure geschützt wird.
 */
class Guarded_Semaphore
	: public Semaphore
{
	// Verhindere Kopien und Zuweisungen
	Guarded_Semaphore(const Guarded_Semaphore&)            = delete;
	Guarded_Semaphore& operator=(const Guarded_Semaphore&) = delete;
public:
	/*! \brief Der Konstruktor reicht nur den Parameter c an den Konstruktor
	 *  der Basisklasse weiter.
	 *
	 *
	 *  \todo Konstruktor implementieren
	 *
	 */
	Guarded_Semaphore(int c)
	{}

	/*! \brief Diese Methode entspricht der gleichnamigen Methode der
	 *  Basisklasse Semaphore, nur dass sie mit Hilfe eines Secure Objekts
	 *  geschützt wird.
	 *
	 *  \todo Methode implementieren
	 *
	 *
	 */
	void p() {
	}

	/// \copydoc p()
	void v() {
	}
};

