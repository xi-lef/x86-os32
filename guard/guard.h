// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Diese Datei enthält die Klasse Guard.
 */

#pragma once

#include "guard/gate.h"
#include "object/queue.h"

/*! \brief Synchronisation des BS-Kerns mit Unterbrechungen.
 *  \ingroup interrupts
 *
 *
 *  Die Klasse Guard dient der Synchronisation zwischen "normalen"
 *  Kernaktivitäten (zur Zeit Ausgaben, später Systemaufrufe) und
 *  Unterbrechungsbehandlungsroutinen. Dazu besitzt Guard eine Warteschlange
 *  (Queue-Objekt) je CPU, in die Gate-Objekte eingereiht werden können. Das ist
 *  immer dann erforderlich, wenn zum Zeitpunkt des Auftretens einer
 *  Unterbrechung der kritische Abschnitt gerade besetzt ist, die epilogue()
 *  Methode also nicht sofort bearbeitet werden darf. Die angesammelten
 *  Epiloge werden behandelt, sobald der kritische Abschnitt wieder
 *  freigegeben wird.
 *
 *  \b Hinweise:
 *  <ul>
 *      <li>Die Epilogqueue stellt eine zentrale Datenstruktur dar, deren
 *      Konsistenz geeignet gesichert werden muß. Die von uns durch die Klasse
 *      Queue bereitgestellte Implementierung ist nicht
 *      unterbrechungstransparent!  Entweder ihr implementiert also selbst eine
 *      unterbrechungstransparente Queue, oder ihr synchronisiert die
 *      bereitgestellte Queue entsprechend hart.</li>
 *
 *      <li>In MPStuBS benötigt man für jeden Prozessor eine eigene
 *      Epilogqueue, über die jeder Prozessor "seine" Epiloge serialisiert.
 *      Allerdings könnten dann Epiloge auf unterschiedlichen Kernen parallel
 *      laufen, da der kritische Abschnitt prozessorweise getrennt
 *      verwaltet wird. Dies muss verhindert werden, indem man beim Betreten
 *      des kritischen Abschnittes diesen mit einem Spinlock vor paralleler
 *      Abarbeitung auf anderen Prozessoren schützt. Dieses Spinlock ist nicht
 *      zu verwechseln mit der Sperrvariable, die das Betreten der Epilogebene
 *      markiert!</li>
 *
 *	    <li>Da Gate Objekte nur einen einzigen Verkettungszeiger besitzen,
 *	    dürfen sie zu einem Zeitpunkt nur ein einziges Mal in der Epilogliste
 *	    aufgeführt sein. Wenn also zwei gleichartige Interrupts so schnell
 *	    aufeinanderfolgen, dass der zugehörige Epilog noch gar nicht behandelt
 *	    wurde, darf nicht versucht werden, dasselbe Gate Objekt zweimal in die
 *	    Epilogliste einzutragen. Die Klasse Gate  bietet Methoden, dies zu
 *	    vermerken bzw. zu prüfen.</li>
 *
 *	    <li>Ein Betriebssystem sollte Unterbrechungen immer nur so kurz wie
 *	    möglich sperren. Daher sieht das Pro-/Epilog-Modell vor, dass Epiloge
 *	    durch Prologe unterbrochen werden können. Für OOStuBS bedeutet das,
 *	    dass bereits vor der Ausführung des Epilogs einer
 *	    Unterbrechungsbehandlung Interrupts wieder zugelassen werden
 *	    sollen.</li>
 *    </ul>
 */
class Guard {
	// Verhindere Kopien und Zuweisungen
	Guard(const Guard&) = delete;
	Guard& operator=(const Guard&) = delete;
public:
	/*! \brief Konstruktor
	 *
	 */
	Guard () {}

public:
	/*! \brief Betreten des kritischen Abschnitts.
	 *
	 *  Das Betreten des kritischen Abschnittes ist je nach Art des Systems
	 *  unterschiedlich zu handhaben. Bei einem Uniprozessorsystem genügt es,
	 *  das Betreten durch einfaches Setzen einer Sperrvariable zu markieren,
	 *  da nur ein einziger Kontrollfluss gleichzeitig den kritischen Bereich
	 *  betreten kann. Sobald jedoch mehrere CPUs vorhanden sind, ist dies
	 *  nicht mehr der Fall. Will nun eine CPU den kritischen Bereich betreten,
	 *  obwohl sich dort schon eine andere CPU befindet, so soll hier aktiv
	 *  gewartet werden, bis der kritische Bereich wieder freigegeben wurde.
	 *
	 *  \todo Methode implementieren
	 */
	void enter();
	/*! \brief Verlassen des kritischen Abschnitts.
	 *
	 *  Mit dieser Methode wird der kritische Abschnitt verlassen und die
	 *  angestauten Epiloge werden abgearbeitet.
	 *
	 *  \todo Methode implementieren
	 */
	void leave();
	/*! \brief Ein Prolog möchte, dass seine Epilog-Aktivität ausgeführt wird.
	 *
	 *  Diese Methode wird von guardian () aufgerufen, falls der zuvor
	 *  ausgeführte Prolog durch einen Rückgabewert true angezeigt hat, dass
	 *  sein Epilog ausgeführt werden soll. Ob der Epilog sofort behandelt oder
	 *  zunächst nur in die Epilogliste eingehängt wird, hängt davon ab, ob der
	 *  kritische Abschnitt frei oder besetzt ist.
	 *
	 *  \todo Methode implementieren
	 */
	void relay(Gate *item);
};

extern Guard guard;
