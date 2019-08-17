// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse WakeUp
 */

#include "guard/gate.h"

/*! \brief Interruptbehandlungsobjekt, um in MPStuBS schlafende Prozessoren
 *  mit einem IPI zu wecken, falls neue Threads aktiv wurden. Diese Interruptbehandlung
 *  soll explizit keinen Epilog nach sich ziehen.
 *
 *  Nur in MPStuBS benötigt.
 */
class WakeUp
	: public Gate
{
public:
	/*! \brief Interruptbehandlung registrieren.
	 *
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void activate();

	/*! \brief Interrupt soll Prozessor nur aufwecken und signalisieren, dass
	 *  neue Threads in der Bereitliste vorhanden sind.
	 *
	 *  \todo Methode implementieren
	 *
	 *
	 */
	bool prologue() override;
};

extern WakeUp wakeup;
