// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Application
 */

#pragma once

#include "thread/thread.h"
#include "debug/output.h"

/*! \brief Die Klasse Application definiert eine Anwendung für OO-Stubs.
 */
class StatusApplication : public Thread {
	// Verhindere Kopien und Zuweisungen
	StatusApplication(const StatusApplication&)            = delete;
	StatusApplication& operator=(const StatusApplication&) = delete;

private:
	int id;

public:
	/*! \brief Konstruktor
	 *
	 * \param i Instanz-ID
	 */
    StatusApplication(void *tos, int i = 0) : Thread(tos), id(i) {}

	/*! \brief Enthält den Code der Anwendung
	 *
	 */
	void action();

    /*! \brief Setzt eine Instanz-ID
     *
     */
    void setID(int i);
};

