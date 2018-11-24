// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Ticketlock
 */

#pragma once

#include "types.h"

/*! \brief Mit Hilfe eines Ticketlocks kann man Codeabschnitte serialisieren,
 *  die echt nebenläufig auf mehreren CPUs laufen.
 *
 *  Die Synchronisation läuft dabei über eine Sperr- und eine Ticket-Variable.
 *  Sobald jemand den kritischen Abschnitt betreten will, holt er sich sein
 *  Ticket durch atomares Erhöhen der Ticket-Variable und wartet darauf, dass
 *  die Sperr-Variable seinen Ticket-Zählerstand erreicht. Verlässt er den
 *  kritischen Abschnitt, so erhöht er die Sperr-Variable und der nächste ist
 *  an der Reihe.
 *
 *  <a href="http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html">Eintrag im GCC Manual über Atomic Builtins</a>
 */
class Ticketlock {
private:
	Ticketlock(const Ticketlock& copy); //verhindert Kopieren

    volatile uint8_t l;
    uint8_t t;

public:

	/*! \brief Konstruktor; Initialisierung des Ticketlocks.
	 *
	 * \opt Konstruktor vervollständigen
	 */
	Ticketlock() : l(0), t(0) {}

	/*! \brief Betritt den gesperrten Abschnitt. Ist dieser besetzt, so wird
	 *  solange aktiv gewartet, bis er betreten werden kann.
	 *
	 * \opt Methode implementieren
	 */
	void lock() {
        uint8_t old = __sync_fetch_and_add(&t, 1);
        while (l != old) ;
	}

	/*! \brief Gibt den gesperrten Abschnitt wieder frei.
	 *
	 * \opt Methode implementieren
	 */
	void unlock() {
        __sync_fetch_and_add(&l, 1);
	}
};

