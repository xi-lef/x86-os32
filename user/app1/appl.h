// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Application
 */

#pragma once

/*! \brief Die Klasse Application definiert eine Anwendung für OO-Stubs.
 */
class Application
{
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

	/*! \brief Enthält den Code der Anwendung
	 *
	 */
	void action ();
};

