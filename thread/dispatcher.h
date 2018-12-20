// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *
 *  \brief Enthält die Klasse Dispatcher.
 */

#include "thread/thread.h"

/*! \brief Der Dispatcher lastet Threads ein und setzt damit die Entscheidungen der Ablaufplanung durch.
 *  \ingroup thread
 *
 *
 *  \note
 *  Der Dispatcher verwaltet den Life-Pointer, der die jeweils aktive Koroutine
 *  angibt und führt die eigentlichen Prozesswechsel durch. In der
 *  Uniprozessorvariante wird nur ein einziger Life-Pointer benötigt, da
 *  lediglich ein Prozess auf einmal aktiv sein kann. Für die
 *  Mehrprozessorvariante wird hingegen für jede CPU ein eigener Life-Pointer
 *  benötigt.
 *
 */
class Dispatcher
{
	// Verhindere Kopien und Zuweisungen
	Dispatcher(const Dispatcher&)            = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

protected:
	void setActive(Thread* c) {
	}
public:
	/*! \brief Konstruktor
	 *
	 *  Der Konstruktor initialisiert den oder die Life-Pointer mit Null,
	 *  um anzuzeigen, dass auf der jeweiligen CPU noch keine Koroutine bekannt
	 *  ist.
	 *
	 *  \todo Konstruktor implementieren
	 *
	 */
	Dispatcher()
	{
	}

	/*! \brief Hiermit kann abgefragt werden, welche Koroutine gerade im Besitz
	 *  des aktuellen Prozessors ist.
	 *
	 *
	 *  \todo Methode implementieren
	 *
	 */
	Thread* active() {
		return 0;
	}

	/*! \brief Mit dieser Methode wird die Koroutine first im Life-Pointer des
	 *  aktuellen Prozessores vermerkt und gestartet.
	 *  \param first Erste auf dem aktuellen Prozessor zu startende Koroutine
	 *   im System.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void go(Thread *first);
	/*! \brief Diese Methode setzt den Life-Pointer des aktuellen Prozessors auf
	 *  next und führt einen Koroutinenwechsel vom alten zum neuen Life-Pointer
	 *  durch.
	 *  \param next Nächste auszuführende Koroutine.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void dispatch(Thread *next);

	/*! \brief Funktion zum Starten eines Threads.
	 *
	 *  Für die allererste Aktivierung eines Threads muss eine "Rücksprungadresse"
	 *  zu einer Funktion bekannt sein, von der aus dann der Übergang von der C- zur
	 *  C++-Ebene erfolgen kann. Hierfür dient die Funktion kickoff.
	 *
	 *  <b>Aktivierung von kickoff</b>
	 *  Bei der Initialisierung eines Threads mit Hilfe der Funktion toc_settle()
	 *  wird nicht nur die Struktur toc für den ersten Threadwechsel
	 *  vorbereitet, sondern auch die Adresse der Funktion kickoff als
	 *  Rücksprungadresse auf den Stack gelegt. Beim ersten Threadwechsel
	 *  mit toc_switch wird daher an den Anfang von kickoff "zurück"gesprungen,
	 *  womit die Ausführung beginnt.
	 *
	 *  Diese Methode realisiert den Sprung von der C-Ebene zur C++-Ebene, indem sie
	 *  ganz einfach die Methode action() des als Parameter vorgefundenen
	 *  Threadobjektes aufruft.
	 *
	 *  \b Hinweis
	 *  Da diese Funktion nicht wirklich aufgerufen, sondern nur durch eine
	 *  geschickte Initialisierung des Stacks der Koroutine angesprungen wird, darf
	 *  sie nie terminieren. Anderenfalls würde ein sinnloser Wert als
	 *  Rücksprungadresse interpretiert werden und der Rechner abstürzen.
	 *
	 *  \param object Thread, der gestartet werden soll.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	static void kickoff(Thread* object);
};

