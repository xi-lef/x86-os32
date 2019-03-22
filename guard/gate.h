// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Gate
 */

#pragma once

#include "object/queuelink.h"

/*! \brief Klasse von Objekten, die in der Lage sind, Unterbrechungen zu behandeln.
 *  \ingroup interrupts
 *
 *
 * Die Klassen aller Objekte, die in der Plugbox eingetragen werden sollen,
 * müssen von Gate abgeleitet werden.
 *
 * Alle diese Objekte müssen die virtuelle Methode prologue() definieren.
 */
class Gate {
	// Verhindere Kopien und Zuweisungen
private:
	Gate(const Gate&) = delete;
	Gate& operator=(const Gate&) = delete;

    bool in_queue = false;

public:
    QueueLink<Gate> queue_link;

	/*! \brief Konstruktor
	 *
	 */
	Gate() {}

	/*! \brief Destruktor
	 *
	 *  Klassen mit virtuellen Methoden sollten grundsätzlich einen virtuellen
	 *  Destruktor besitzen, der auch leer sein darf. Bei OO-Stubs beruhigt
	 *  das den Compiler, bei anderen Systemen garantiert das, dass die
	 *  Speicherfreigabe mit delete für Objekte der abgeleiteten Klassen
	 *  korrekt funktioniert.
	 */
	virtual ~Gate() {}

	/*! \brief Unterbrechungsbehandlungsroutine, die sofort nach Auftreten der
     *  Unterbrechung asynchron zu anderen Kernaktivitäten ausgeführt wird.
	 *
     *  \return gibt an, ob der entsprechende Epilog auszuführen ist.
     */
    virtual bool prologue() = 0;

    /*! \brief Eine gegebenenfalls verzögerte, synchronisiert
     *  ausgeführte Unterbrechungsbehandlung.
     *
     *  \todo virtuelle Methode definieren
     */
    virtual void epilogue() {}

    /*! \brief Setzt atomar ein Flag um zu markieren, dass sich das Objekt
     *  gerade in einer Epilog-Warteschlange befindet.
     *
     *  \todo Methode implementieren
     *
     *  \return Gibt false zurück, falls das Flag vorher bereits gesetzt
     *  war, andernfalls true.
     */
    bool set_queued() {
        return __sync_lock_test_and_set(&in_queue, true);
    }

    /*! \brief Setzt das in set_queued() gesetzte Flag zurück.
     *
     *  \todo Methode implementieren
     */
    void set_dequeued() {
        __sync_lock_release(&in_queue);
    }
};

