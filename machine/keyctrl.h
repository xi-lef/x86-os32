// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält Klasse Keyboard_Controller
 */

#pragma once

#include "machine/io_port.h"
#include "machine/keydecoder.h"
#include "machine/key.h"

/*! \brief Abstraktion für den Tastaturcontroller des PCs
 *  \ingroup io
 *
 *
 *  Die Klasse Keyboard_Controller dient dazu, die PC Tastatur zu initialisieren
 *  und aus den gesendeten Make- und Break-Codes den Scan- und ASCII Code der
 *  gedrückten Taste zu bestimmen.
 */
class Keyboard_Controller
{
	// Verhindere Kopien und Zuweisungen
	Keyboard_Controller(const Keyboard_Controller&)            = delete;
	Keyboard_Controller& operator=(const Keyboard_Controller&) = delete;

protected:
	/*! \brief Sendet einen Befehl an den Tastaturcontroller.
	 *
	 *  Ein Tastaturbefehl besteht aus Tastaturkommando und einem
	 *  zugehörigem Datumsbyte. Zur Implementierung soll die
	 *  send_byte() Methode verwendet werden.
	 *
	 *  \todo Methode implementieren
	 *
	 *  \param cmd    abzusetzender Befehl
	 *  \param data   zu sendende Daten
	 */
	void send_command(unsigned char cmd, unsigned char data);

	/*! \brief Sendet ein Datum an den Tastaturcontroller.
	 *
	 *  Sobald der Controller alle von der CPU geschriebenen Zeichen abgeholt
	 *  hat, kann der Befehl und anschließend die Daten übermittelt werden.
	 *
	 *  \todo Methode implementieren
	 *
	 *  \param byte zu sendende Daten
	 */
	void send_byte(unsigned char byte);

	/*! \brief Leert den Tastaturpuffer vollständig.
	 *
	 *  Die Tastatur schickt u.U. keine weiteren Interrupts wenn nicht alle
	 *  Daten abgeholt werden. Deswegen kann mit dieser Funktion sichergestellt
	 *  werden, dass immer neue Interrupts kommen, auch wenn dabei Tastendrücke
	 *  verloren gehen könnten.
	 *
	 *  \todo Methode implementieren
	 */
	void drainKeyboardBuffer();

private:
	Key_Decoder keydecoder;
	char leds;

	// Benutzte Ports des Tastaturcontrollers
	const IO_Port ctrl_port; // Status- (R) u. Steuerregister (W)
	const IO_Port data_port; // Ausgabe- (R) u. Eingabepuffer (W)

	/// Bits im Statusregister
	enum { outb = 0x01, inpb = 0x02, auxb = 0x20 };

	/// Kommandos an die Tastatur
	struct kbd_cmd {
		enum { set_led = 0xed, set_speed = 0xf3 };
	};
	enum { cpu_reset = 0xfe };

	/// Antworten der Tastatur
	struct kbd_reply {
		enum { ack = 0xfa };
	};

public:

	/*! \brief Konstruktor; Initialisierung der Tastatur.
	 *
	 *  Alle LEDs werden ausgeschaltet und die Wiederholungsrate auf maximale
	 *  Geschwindigkeit eingestellt.
	 */
	Keyboard_Controller ();

	/*! \brief Dient der Tastaturabfrage nach dem Auftreten einer Tastatur-
	 *  unterbrechung.
	 *
	 *  Zur Dekodierung kann Key_Decoder::decode() verwendet werden.
	 *
	 *  Wenn der Tastendruck abgeschlossen ist und ein Scancode,
	 *  sowie gegebenenfalls ein ASCII Code emittelt werden konnte, werden diese
	 *  in Key zurückgeliefert. Wenn dagegen bloß eine der Spezialtasten Shift,
	 *  Alt, CapsLock usw. gedrückt wurde, dann liefert key_hit() einen ungültigen
	 *  Wert zurück, was mit Key::valid() überprüft werden kann.
	 *
	 *  \todo Methode implementieren (u.a. enum \c outb verwenden!)
	 *
	 *  \return Dekodierte Taste
	 */
	Key key_hit ();

	/*! \brief Führt einen Neustart des Rechners durch. Ja, beim PC macht das der
	 *  Tastaturcontroller.
	 *
	 */
	void reboot ();

	/*! \brief Funktion zum Einstellen der Wiederholungsrate der Tastatur.
	 * \param delay bestimmt, wie lange eine Taste gedrückt werden muss, bevor
	 * die Wiederholung einsetzt. Erlaubt sind Werte von \c 0 (250ms),
	 * \c 1 (500ms), \c 2 (750ms) und \c 3 (1000ms).
	 * \param speed bestimmt, wie schnell die Tastencodes aufeinander folgen
	 * soll. Erlaubt sind Werte zwischen \c 0 (30 Zeichen pro Sekunde) und
	 * \c 31 (2 Zeichen pro Sekunde).
	 *
	 *  \todo Methode implementieren  (Verwendung von send_command())
	 */
	void set_repeat_rate (int speed, int delay);

	/// Namen der LEDs
	enum led_t {
		led_caps_lock = 4,      ///< Caps Lock
		led_num_lock = 2,       ///< Num Lock
		led_scroll_lock = 1     ///< Scroll Lock
	};

	/*! \brief Setzt oder löscht die angegebene Leuchtdiode.
	 *  \param led Gibt an, welche LED geschaltet werden soll.
	 *  \param on LED an- oder ausschalten.
	 *
	 *  \todo Methode implementieren (Verwendung von send_command())
	 */
	void set_led (led_t led, bool on);
};

