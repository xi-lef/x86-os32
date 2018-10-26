// vim: set et ts=4 sw=4:

/* INCLUDES */

#include "machine/keyctrl.h"
#include "debug/output.h"

/* GLOBALE VARIABLEN */

/* OEFFENTLICHE METHODEN */

// KEYBOARD_CONTROLLER: Initialisierung der Tastatur: alle LEDs werden
//                      ausgeschaltet und die Wiederholungsrate auf
//                      maximale Geschwindigkeit eingestellt.

Keyboard_Controller::Keyboard_Controller () :
	keydecoder(this), ctrl_port (0x64), data_port (0x60)
{
	// alle LEDs ausschalten (bei vielen PCs ist NumLock nach dem Booten an)
	set_led (led_caps_lock, false);
	set_led (led_scroll_lock, false);
	set_led (led_num_lock, false);

	// maximale Geschwindigkeit, minimale Verzoegerung
	set_repeat_rate (0, 0);
}

// KEY_HIT: Dient der Tastaturabfrage nach dem Auftreten einer Tastatur-
//          unterbrechung. Wenn der Tastendruck abgeschlossen ist und
//          ein Scancode, sowie gegebenenfalls ein ASCII Code emittelt
//          werden konnte, werden diese in Key zurueckgeliefert. Anderen-
//          falls liefert key_hit () einen ungueltigen Wert zurueck, was
//          mit Key::valid () ueberprueft werden kann.

Key Keyboard_Controller::key_hit()
{
	//TODO: In dieser Methode muss noch Code von euch ergänzt werden!
	Key pressed;  // nicht explizit initialisierte Tasten sind ungueltig

	return pressed;
}

// REBOOT: Fuehrt einen Neustart des Rechners durch. Ja, beim PC macht
//         das der Tastaturcontroller.

void Keyboard_Controller::reboot ()
{
	const IO_Port CMOS_ctrl(0x70);
	const IO_Port CMOS_data(0x71);
	const IO_Port system_control_portA(0x92);
	DBG_VERBOSE << "rebooting smp" << endl;
	CMOS_ctrl.outb(0xe + 1);
	CMOS_data.outb(0);
	system_control_portA.outb(0x3);
}
// SET_REPEAT_RATE: Funktion zum Einstellen der Wiederholungsrate der
//                  Tastatur. delay bestimmt, wie lange eine Taste ge-
//                  drueckt werden muss, bevor die Wiederholung einsetzt.
//                  Erlaubt sind Werte zwischen 0 (minimale Wartezeit)
//                  und 3 (maximale Wartezeit). speed bestimmt, wie
//                  schnell die Tastencodes aufeinander folgen soll.
//                  Erlaubt sind Werte zwischen 0 (sehr schnell) und 31
//                  (sehr langsam).

void Keyboard_Controller::set_repeat_rate (int speed, int delay)
{
	//TODO: In dieser Methode muss noch Code von euch ergänzt werden!
	(void) speed;
	(void) delay;
}

// SET_LED: setzt oder loescht die angegebene Leuchtdiode

void Keyboard_Controller::set_led (led_t led, bool on)
{
	//TODO: In dieser Methode muss noch Code von euch ergänzt werden!
	(void) led;
	(void) on;
}

void Keyboard_Controller::drainKeyboardBuffer()
{
	//TODO: In dieser Methode muss noch Code von euch ergänzt werden!
}

void Keyboard_Controller::send_command(unsigned char cmd, unsigned char data)
{
	//TODO: In dieser Methode muss noch Code von euch ergänzt werden!
	(void) cmd;
	(void) data;
}

void Keyboard_Controller::send_byte(unsigned char byte)
{
	//TODO: In dieser Methode muss noch Code von euch ergänzt werden!
	(void) byte;
}
