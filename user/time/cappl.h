// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Application
 */

#pragma once

#include "thread/thread.h"
#include "debug/output.h"

/*! \brief Die Klasse Application definiert eine Anwendung für OO-Stubs.
 */
class ClockApplication : public Thread {
	// Verhindere Kopien und Zuweisungen
	ClockApplication(const ClockApplication&)            = delete;
	ClockApplication& operator=(const ClockApplication&) = delete;

private:
	int id;

public:
	/*! \brief Konstruktor
	 *
	 * \param i Instanz-ID
	 */
    ClockApplication(void *tos, int i = 0) : Thread(tos), id(i) {}
    ClockApplication(int i = 0) : Thread(), id(i) {}

	/*! \brief Enthält den Code der Anwendung
	 *
	 */
	void action() override;

    /*! \brief Setzt eine Instanz-ID
     *
     */
    void setID(int i);
};

