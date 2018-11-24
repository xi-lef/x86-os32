// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Plugbox
 */

#pragma once

#include "guard/gate.h"

/*! \brief Abstraktion einer Interruptvektortabelle.
 *  \ingroup interrupts
 *
 *
 *  Damit kann man die Adresse der Behandlungsroutine für jeden Hardware-
 *  und Softwareinterrupt und jede Prozessorexception festlegen. Jede
 *  Unterbrechungsquelle wird durch ein Gate-Objekt repräsentiert. Diese
 *  liegen in einem Feld (256 Elemente). Der Index in dieses Feld ist dabei
 *  die Vektornummer.
 */
class Plugbox
{
private:
	// Verhindere Kopien und Zuweisungen
	Plugbox(const Plugbox&)            = delete;
	Plugbox& operator=(const Plugbox&) = delete;
public:
	/*! \brief Liste der verwendeten Interruptvektoren
	 *
	 *  In OOStuBS benötigen wir lediglich Vektornummern für den Timerbaustein
	 *  und die Tastatur, in MPStuBS noch weitere zur Synchronisation zwischen
	 *  CPUs.
	 */
	enum Vector {
		timer = 32,         ///< Interruptvektornummer für den Timerinterrupt.
		keyboard = 33,      ///< Interruptvektornummer für den Keyboardinterrupt.
		assassin = 100,     ///< Interruptvektornummer für den Assassin; nur in MPStuBS benötigt.
		wakeup = 101,       ///< Interruptvektornummer zum Aufwecken von schlafenden CPUs; nur in MPStubs benötigt.
        panic = 255         // panic!!1
	};
	/*! \brief Initialisierung der Gate map mit einem Default Gate.
	 *
	 *  Im Konstruktor wird für alle Unterbrechungen und Ausnahmen das global
	 *  bekannte Panic Objekt panic eingetragen, das eine minimale
	 *  Unterbrechungsbehandlung sicherstellt.
	 *
	 *  \todo Konstruktor implementieren
	 *
	 */
	Plugbox();
	/*! \brief Einstöpseln einer Behandlungsroutine, die in Form eines
	 *  Gate-Objekts vorhanden sein muss.
	 *  \param vector Interruptvektor, für den die Behandlungsroutine registriert
	 *  werden soll.
	 *  \param gate Objekt mit Behandlungsroutine
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void assign(unsigned int vector, Gate *gate);
	/*! \brief Abfrage eines eingetragenen Gate Objekts.
	 *
	 *  \param vector Abzufragende Interruptvektornummer
	 *  \return Referenz auf das mit \b vector assoziierte Gate Objekt
	 *
	 *  \todo Methode implementieren
	 *
	 */
	Gate* report(unsigned int vector);
};

extern Plugbox plugbox;
