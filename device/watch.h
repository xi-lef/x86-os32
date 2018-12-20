// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Watch
 */

#pragma once

/* INCLUDES */

#include "types.h"
#include "guard/gate.h"

/*! \brief Interruptbehandlung für Timerinterrupts.
 *
 *  Die Klasse Watch sorgt für die Behandlung der Zeitgeberunterbrechungen,
 *  indem sie eine Zeitscheibe verwaltet und gegebenenfalls einen Threadwechsel
 *  auslöst.
 */
class Watch
	: public Gate
{
	// Verhindere Kopien und Zuweisungen
	Watch(const Watch&)            = delete;
	Watch& operator=(const Watch&) = delete;

public:
	Watch() {}

	/*! \brief Uhr "aufziehen"
	 *
	 *  Das Watch Objekt muss sich bei der Plugbox anmelden und
	 *  sich so initialisieren, dass beim Aufruf von Watch::activate()
	 *  ca. alle \b us Mikrosekunden regelmäßig Unterbrechungen auslöst werden.
	 *  Hierfür muss anhand der mit LAPIC::timer_ticks() bestimmten
	 *  Timerfrequenz ein passender Timer-Divisor bestimmt werden, welcher
	 *  möglichst klein, aber groß genug ist, um einen Überlauf des
	 *  32bit-Zählers auszuschließen. Durch den Aufruf von Watch::interval()
	 *  soll \b us wieder abfragbar sein.
	 *  \param us Gewünschtes Unterbrechungsintervall in Mikrosekunden.
	 *  \return Gibt an ob das Interval eingestellt werden konnte.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	bool windup(uint32_t us);

	/*! \brief Enthält den Prolog der Unterbrechungsbehandlung.
	 *
	 *  \return Gibt \b true zurück, falls ein Epilog notwendig ist, ansonsten
	 *  \b false.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	bool prologue();

	/*! \brief In dieser Methode wird der Threadwechsel ausgelöst.
	 *
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void epilogue();

	/*! \brief Gibt an, welches Unterbrechungsintervall eingestellt wurde.
	 *  \return Eingestelltes Unterbrechungsintervall
	 *
	 *  \todo Methode implementieren
	 *
	 */
	uint32_t interval();

	/*! \brief Startet den CPU-lokalen Timer.
	 *
	 * Der CPU-lokale Timer soll mit dem vorab in windup() konfigurierten
	 * Interval gestartet werden. Um Timer-Interrupts auf allen CPUs zu
	 * erhalten, muss diese Methode pro %CPU einmal aufgerufen werden.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void activate();

};

