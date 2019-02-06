// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *
 *  \brief Enthält die Klasse Scheduler
 */

#include "thread/dispatcher.h"
#include "thread/thread.h"
#include "object/queue.h"

/*! \brief Der Scheduler implementiert die Ablaufplanung und somit die Auswahl des nächsten Threads.
 *  \ingroup thread
 *
 *
 *  \note Der Scheduler verwaltet die Ready-Liste (ein privates Queue Objekt der
 *  Klasse), also die Liste der lauffähigen Threads. Die
 *  Liste wird von vorne nach hinten abgearbeitet. Dabei werden Threads, die
 *  neu im System sind oder den Prozessor abgeben, stets an das Ende der Liste
 *  angefügt.
 */
class Scheduler
	: public Dispatcher
{
	// Verhindere Kopien und Zuweisungen
	Scheduler(const Scheduler&)            = delete;
	Scheduler& operator=(const Scheduler&) = delete;

    //friend class Guarded_Scheduler;

private:
    Queue<Thread> ready_list;

public:
    Thread *idlethread[CPU_MAX];

	/*! \brief Konstruktor
	 *
	 */
	Scheduler() {}

	/*! \brief Starten des Schedulings
	 *
	 *  Diese Methode setzt das Scheduling in Gang, indem der erste
	 *  Thread von der Ready-Liste entfernt und aktiviert wird. In MPStuBS muss
	 *  diese Methode auf jeder CPU einmal aufgerufen werden, um auf dem
	 *  jeweiligen Prozessor den ersten Thread einzulasten.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void schedule();

	/*! \brief Anmelden eines Threads zum Scheduling
	 *
	 *  Mit dieser Methode wird der Thread that beim Scheduler angemeldet. Er
	 *  wird an das Ende der Ready-Liste angefügt.
	 *  \param that Thread, der angemeldet werden soll.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void ready(Thread *that);

	/*! \brief Selbstbeenden des aktuellen Threads.
	 *
	 *  Hiermit kann sich ein Thread selbst beenden. Er wird nun nicht wieder
	 *  an das Ende der Ready-Liste angefügt. Statt dessen wird nur der erste
	 *  Thread von der Ready-Liste heruntergenommen und aktiviert.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void exit();

	/*! \brief Beenden eines beliebigen Threads
	 *
	 *  Mit dieser Methode kann ein Thread einen anderen (\b that) beenden.
	 *  In OOStuBS genügt es einfach den Thread \b that von der Ready-Liste
	 *  zu entfernen. Er wird somit nie wieder vom Prozessor ausgeführt werden.
	 *  In MPStuBS ist die Implementierung etwas kniffliger, da der Thread
	 *  \b that auch während der Ausführung von kill auf anderen Prozessoren
	 *  laufen kann. Wird \b that nicht in der Ready-Liste gefunden, so muss
	 *  bei ihm vermerkt werden, dass der
	 *  Thread beendet werden soll. Dies muss in resume überprüft werden,
	 *  bevor ein Thread wieder in die Ready-Liste eingetragen wird.
	 *  \param that Thread, der beendet werden soll.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void kill(Thread *that);

	/*! \brief Auslösen eines Threadwechsels
	 *
	 *  Hiermit kann ein Threadwechsel ausgelöst werden, ohne dass der
	 *  aufrufende Thread wissen muss, welche anderen Thread Objekte im System
	 *  existieren und welcher davon sinnvollerweise aktiviert werden sollte.
	 *  Diese Entscheidung trifft der Scheduler anhand der Einträge seiner
	 *  Ready-Liste. Der Scheduler soll den gerade laufenden Thread
	 *  an das Ende der Ready-Liste anfügen und den ersten Thread in der
	 *  Ready-Liste aktivieren.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void resume();

    void block(Waitingroom *w);

    bool is_empty();

    void set_idle_thread(int cpuid, Thread *thread);

    void wakeup(Thread *customer);
};

extern Scheduler scheduler;
