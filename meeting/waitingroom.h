// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse Waitingroom.
 */

/*! \defgroup ipc IPC Subsystem
 *
 * Das IPC Subsystem stellt der Fadenabstraktion eine Schnittstelle
 * bereit um Synchronisation durchzuführen. Diese Synchronisation
 * beinhaltet sowohl die Synchronisation zwischen Threads
 * (Semaphoren), als auch die Synchronisation mit der Umwelt (Bell).
 */

#include "object/queue.h"
class Thread;

/*! \brief Liste von Threads, die auf ein Ereignis warten.
 *  \ingroup ipc
 *
 *
 *  Die Klasse Waitingroom implementiert eine Liste von Threads, die alle auf
 *  ein bestimmtes Ereignis warten.
 *  \note
 *  Die Methode remove(Thread*) muss virtuell sein, damit der Scheduler einen
 *  Thread aus dem Wartezimmer entfernen kann, ohne wissen zu müssen, welcher
 *  Art dieses Wartezimmer ist. Sofern es erforderlich ist, kann eine von
 *  Waitingroom abgeleitete Klasse die Methode auch noch neu definieren.
 *
 *  Der Destruktor sollte wie bei allen Klassen, die virtuelle Methoden
 *  definieren, ebenfalls virtuell sein.
 */
class Waitingroom
	: public Queue<Thread>
{
	// Verhindere Kopien und Zuweisungen
	Waitingroom(const Waitingroom&)            = delete;
	Waitingroom& operator=(const Waitingroom&) = delete;

public:
	Waitingroom() {}

	/*! \brief Destruktor
	 *
	 *  Der Destruktor entfernt alle noch wartenden Threads von der Liste und
	 *  weckt sie zudem auf.
	 *
	 *  \todo Destruktor implementieren
	 *
	 */
	virtual ~Waitingroom();

	/*! \brief Mit dieser Methode kann der angegebene Thread customer vorzeitig
	 *  aus dem Wartezimmer entfernt werden.
	 *
	 *
	 *  \todo Methode implementieren
	 *
	 */
	virtual void remove(Thread *customer);
};

