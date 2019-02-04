// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse Bell.
 */

#include "meeting/waitingroom.h"
#include "object/queuelink.h"
class Bellringer;

/*! \brief Synchronisationsobjekt zum Schlafenlegen für eine bestimmte
 *  Zeitspanne
 *  \ingroup ipc
 *
 *
 *  Ein "Wecker" ist ein Synchronisationsobjekt, mit dem ein oder mehrere
 *  Threads sich für eine bestimmte Zeit schlafen legen können.
 */
class Bell
	: public Waitingroom
{
	// Verhindere Kopien und Zuweisungen
	Bell(const Bell&)            = delete;
	Bell& operator=(const Bell&) = delete;

private:

	/* Erlaube Klasse Bellringer Zugriff auf die interne Variable counter und
	 * bellringer_link. */
	friend class Bellringer;

public:
	/*! \brief Konstruktor.
	 *
	 *  Der Wecker ist zunächst noch abgeschaltet.
	 *
	 *  \todo Konstruktor implementieren
	 *
	 */
	Bell()
	{}

	/*! \brief Läuten der Glocke
	 *
	 *  Wird von Bellringer aufgerufen, wenn die Wartezeit abgelaufen ist und
	 *  weckt den schlafenden Thread.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void ring();

	/*! \brief Temporäres Bell-Objekt erzeugen und Thread schlafen legen, bis
	 * der Wecker klingelt.
	 *  \param ms Zeit in Millisekunden, die zur Umrechnung an Bellringer::job()
	 *  weitergegeben wird.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	static void sleep(unsigned int ms);
};

