// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Bellringer.
 */

#pragma once

#include "meeting/bell.h"
#include "object/queue.h"
/*! \brief Verwaltung und Anstoßen von zeitgesteuerten Aktivitäten.
 *  \ingroup ipc
 *
 *
 *  Der "Glöckner" (Bellringer) wird regelmäßig aktiviert und prüft, ob
 *  irgendwelche "Glocken" (Bell-Objekte) läuten müssen. Die Glocken befinden
 *  sich in einer Liste, die der Glöckner verwaltet. Eine geschickte
 *  Implementierung vermeidet, bei jeder Aktivierung die gesamte Liste
 *  zu durchlaufen.
 *
 *  Auf diese Weise erreicht man eine Komplexität vom O(1) im Timer
 *  Interrupt, sofern keine Glocke aktiviert werden muss.
 */
class Bellringer
{
	// Verhindere Kopien und Zuweisungen
	Bellringer(const Bellringer&)            = delete;
	Bellringer& operator=(const Bellringer&) = delete;
public:
	/*! \brief Konstruktor.
	 *
	 */
	Bellringer() {}

public:
	/*! \brief Prüft, ob Glocken zu läuten sind und tut es gegebenenfalls.
	 *
	 *  Bei jedem Aufruf von check vergeht ein Tick. Wenn das Ticken einer
	 *  Glocke dazu führt, dass sie ihre Zeit abgelaufen ist, wird sie
	 *  geläutet.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void check();

	/*! \brief Die Glocke \b bell wird dem Glöckner überantwortet. Sie soll nach
	 *  \b ms Millisekunden geläutet werden.
	 *  \param bell Glocke, die nach \b ms Millisekunden geläutet werden
	 *  soll.
	 *  \param ms Millisekunden, die abgewartet werden sollen,
	 *  bevor die Glocke \b bell geläutet wird.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void job(Bell *bell, unsigned int ms);

	/*! \brief Die Glocke \b bell soll nun doch nicht geläutet werden.
	 *  \param bell Die Glocke, die nicht geläutet werden soll.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void cancel(Bell *bell);

	/*! \brief Ist eine Glocke eingehangen?
	 *  \return Mindestens eine Glocke ist eingehangen.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	bool bell_pending();

};

