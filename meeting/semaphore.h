// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse Semaphore.
 */

#include "meeting/waitingroom.h"

/*! \brief Semaphore werden zur Synchronisation von Threads verwendet.
 *  \ingroup ipc
 *
 *
 *  Die Klasse Semaphore implementiert das Synchronisationskonzept des zählenden
 *  Semaphors. Die benötigte Warteliste erbt sie dabei von ihrer Basisklasse
 *  Waitingroom.
 */
class Semaphore
	: public Waitingroom
{
	// Verhindere Kopien und Zuweisungen
	Semaphore(const Semaphore&)            = delete;
	Semaphore& operator=(const Semaphore&) = delete;

private:
    int counter;

public:
	/*! \brief Der Konstruktor initialisiert den Semaphorzähler mit dem
	 *  angegebenen Wert \b c
	 *  \param c Initialer Zählerwert des Semaphors
	 *
	 *  \todo Konstruktor implementieren
	 */
	Semaphore(int c = 0) : counter(c) {}

	/*! \brief Warten auf das Freiwerden eines kritischen Abschnitts.
	 *
	 *  Warteoperation: Sofern der Semaphorzähler größer als Null ist, wird er
	 *  nur um eins erniedrigt. Anderenfalls wird der aktuell laufende Thread
	 *  an die Warteliste angefügt und blockiert.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void p();

	/*! \brief Freigeben des kritischen Abschnitts.
	 *
	 *  Freigabeoperation: Wenn auf der Warteliste mindestens ein Thread
	 *  eingetragen ist, wird der erste davon aufgeweckt. Anderenfalls wird der
	 *  Semaphorzähler um eins erhöht.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void v();
};

