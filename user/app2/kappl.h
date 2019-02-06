// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse KeyboardApplication
 */

#pragma once

#include "thread/thread.h"

/*! \brief Die Klasse KeyboardApplication definiert eine Anwendung für OO-Stubs.
 */
class KeyboardApplication : public Thread {
	// Verhindere Kopien und Zuweisungen
	KeyboardApplication(const KeyboardApplication&)            = delete;
	KeyboardApplication& operator=(const KeyboardApplication&) = delete;

private:
	int id;
public:
	/*! \brief Konstruktor
	 *
	 * \param i Instanz-ID
	 */
    KeyboardApplication(void *tos, int i = 0) : Thread(tos), id(i) {}

	/*! \brief Enthält den Code der Anwendung
	 *
	 */
	void action();

    void setID(int i);
};

