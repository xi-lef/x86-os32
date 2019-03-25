// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Application
 */

#pragma once

#include "thread/thread.h"
#include "debug/output.h"

/*! \brief Die Klasse Application definiert eine Anwendung für OO-Stubs.
 */
class Application : public Thread {
	// Verhindere Kopien und Zuweisungen
	Application(const Application&)            = delete;
	Application& operator=(const Application&) = delete;

private:
	int id;

public:
	/*! \brief Konstruktor
	 *
	 * \param i Instanz-ID
	 */
    Application(void *tos, int i = 0) : Thread(tos), id(i) {}
    Application(int i = 0) : Thread(), id(i) {}

	/*! \brief Enthält den Code der Anwendung
	 *
	 */
	void action() override;

    /*! \brief Setzt eine Instanz-ID
     *
     */
    void setID(int i);
};

