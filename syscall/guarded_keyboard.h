// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse Guarded_Keyboard
 */

#include "device/keyboard.h"

/*! \brief Systemaufrufschnittstelle zur Tastatur
 *
 */
class Guarded_Keyboard : public Keyboard
{
	// Verhindere Kopien und Zuweisungen
	Guarded_Keyboard(const Guarded_Keyboard&)            = delete;
	Guarded_Keyboard& operator=(const Guarded_Keyboard&) = delete;
public:
	Guarded_Keyboard() {}

	/*! \brief Diese Methode entspricht der gleichnamigen Methode der
	 *  Basisklasse Keyboard, nur dass sie mit Hilfe eines Secure Objekts
	 *  geschützt wird.
	 *
	 *  \todo Methode implementieren
	 *
	 *
	 */
	Key getkey();

    Guarded_Keyboard& operator >>(char &c);
    Guarded_Keyboard& operator >>(String &s);
    Guarded_Keyboard& operator >>(long &i);

    void set_repeat_rate(int speed, int delay);
    void set_led(led_t led, bool on);
};

extern Guarded_Keyboard keyboard;
