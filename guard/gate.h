// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Gate
 */

#pragma once

/*! \brief Klasse von Objekten, die in der Lage sind, Unterbrechungen zu behandeln.
 *  \ingroup interrupts
 *
 *
 * Die Klassen aller Objekte, die in der Plugbox eingetragen werden sollen,
 * müssen von Gate abgeleitet werden.
 *
 * Alle diese Objekte müssen die virtuelle Methode trigger() definieren.
 */
class Gate {
	// Verhindere Kopien und Zuweisungen
	Gate(const Gate&) = delete;
	Gate& operator=(const Gate&) = delete;
public:
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
	/*! \brief Unterbrechungsbehandlungsroutine
	 *
	 *  Diese Methode wird sofort nach Auftreten der Unterbrechung asynchron zu
	 *  anderen Kernaktivitäten ausgeführt. Da sie als rein virtuelle Methode
	 *  implementiert ist, muss sie durch die abgeleiteten Klassen spezifiziert
	 *  werden.
	 */
	virtual void trigger() = 0;
};

