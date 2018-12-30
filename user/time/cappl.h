// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Application
 */

#pragma once

#include "thread/thread.h"
#include "debug/output.h"

/*! \brief Die Klasse Application definiert eine Anwendung für OO-Stubs.
 */
class Clock_Application : public Thread {
	// Verhindere Kopien und Zuweisungen
	Clock_Application(const Clock_Application&)            = delete;
	Clock_Application& operator=(const Clock_Application&) = delete;

private:
	int id;

public:
	/*! \brief Konstruktor
	 *
	 * \param i Instanz-ID
	 */
    Clock_Application(void *tos, int i = 0) : Thread(tos), id(i) {}

	/*! \brief Enthält den Code der Anwendung
	 *
	 */
	void action();

    /*! \brief Setzt eine Instanz-ID
     *
     */
    void setID(int i);
};

