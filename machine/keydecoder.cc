// vim: set et ts=4 sw=4:

/* INCLUDES */

#include "machine/keydecoder.h"
#include "machine/keyctrl.h"

/* STATIC MEMERS */

unsigned char Key_Decoder::normal_tab[] = {
	0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 225, 39, '\b',
	0, 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 129, '+', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, 132, '^', 0, '#',
	'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
	0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '<', 0, 0
};

unsigned char Key_Decoder::shift_tab[] = {
	0, 0, '!', '"', 21, '$', '%', '&', '/', '(', ')', '=', '?', 96, 0,
	0, 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 154, '*', 0,
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, 142, 248, 0, 39,
	'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_', 0,
	0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '>', 0, 0
};

unsigned char Key_Decoder::alt_tab[] = {
	0, 0, 0, 253, 0, 0, 0, 0, '{', '[', ']', '}', '\\', 0, 0,
	0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '~', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 230, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0
};

unsigned char Key_Decoder::asc_num_tab[] = {
	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', ','
};
unsigned char Key_Decoder::scan_num_tab[] = {
	8, 9, 10, 53, 5, 6, 7, 27, 2, 3, 4, 11, 51
};

/* PRIVATE METHODEN */

// GET_ASCII_CODE: ermittelt anhand von Tabellen aus dem Scancode und
//                 den gesetzten Modifier-Bits den ASCII Code der Taste.

void Key_Decoder::get_ascii_code (unsigned char code)
{
	// Sonderfall Scancode 53: Dieser Code wird sowohl von der Minustaste
	// des normalen Tastaturbereichs, als auch von der Divisionstaste des
	// Ziffernblocks gesendet. Damit in beiden Faellen ein Code heraus-
	// kommt, der der Aufschrift entspricht, muss im Falle des Ziffern-
	// blocks eine Umsetzung auf den richtigen Code der Divisionstaste
	// erfolgen.
	if (code == 53 && prefix == prefix1) { // Divisionstaste des Ziffernblocks
		gather.ascii ('/');
		gather.scancode (Key::scan::div);
	}

	// Anhand der Modifierbits muss die richtige Tabelle ausgewaehlt
	// werden. Der Einfachheit halber hat NumLock Vorrang vor Alt,
	// Shift und CapsLock. Fuer Ctrl gibt es keine eigene Tabelle.

	else if (gather.num_lock () && !prefix && code>=71 && code<=83) {
		// Bei eingeschaltetem NumLock und der Betaetigung einer der
		// Tasten des separaten Ziffernblocks (Codes 71-83), sollen
		// nicht die Scancodes der Cursortasten, sondern ASCII und
		// Scancodes der ensprechenden Zifferntasten geliefert werden.
		// Die Tasten des Cursorblocks (prefix == prefix1) sollen
		// natuerlich weiterhin zur Cursorsteuerung genutzt werden
		// koennen. Sie senden dann uebrigens noch ein Shift, aber das
		// sollte nicht weiter stoeren.
		gather.ascii (asc_num_tab[code-71]);
		gather.scancode (scan_num_tab[code-71]);
	} else if (gather.alt_right ()) {
		gather.ascii (alt_tab[code]);
		gather.scancode (code);
	} else if (gather.SHIFT ()) {
		gather.ascii (shift_tab[code]);
		gather.scancode (code);
	} else if (gather.caps_lock ()) {
		// Die Umschaltung soll nur bei Buchstaben gelten
		if ((code>=16 && code<=26) || (code>=30 && code<=40)
				|| (code>=44 && code<=50)) {
			gather.ascii (shift_tab[code]);
			gather.scancode (code);
		} else {
			gather.ascii (normal_tab[code]);
			gather.scancode (code);
		}
	} else {
		gather.ascii (normal_tab[code]);
		gather.scancode (code);
	}
}

/* OEFFENTLICHE METHODEN */

// KEY_DECODED: Interpretiert die Make und Break-Codes der Tastatur und
//              liefert den ASCII Code, den Scancode und Informationen
//              darueber, welche zusaetzlichen Tasten wie Shift und Ctrl
//              gedrueckt wurden. Die Rueckgabe einer validen Taste bedeutet,
//              dass das Zeichen komplett ist, anderenfalls fehlen noch Make
//              oder Breakcodes.
Key Key_Decoder::decode (unsigned char code)
{
	// Taste invalidieren
	gather.invalidate();

	// Die Tasten, die bei der MF II Tastatur gegenueber der aelteren
	// AT Tastatur hinzugekommen sind, senden immer erst eines von zwei
	// moeglichen Prefix Bytes.
	if (code == prefix1 || code == prefix2) {
		prefix = code;
	}
	// Das Loslassen einer Taste ist eigentlich nur bei den "Modifier" Tasten
	// SHIFT, CTRL und ALT von Interesse, bei den anderen kann der Break Code
	// ignoriert werden.
	else {
		if (code & break_bit) {
			code &= ~break_bit;
			// Der Break Code einer Taste ist gleich dem
			// Make Code mit gesetzten break_bit.
			switch (code) {
				case 42:
				case 54:
					gather.SHIFT (false);
					break;
				case 56:
					if (prefix == prefix1)
						gather.alt_right (false);
					else
						gather.alt_left (false);
					break;
				case 29:
					if (prefix == prefix1)
						gather.ctrl_right (false);
					else
						gather.ctrl_left (false);
					break;
			}

		} else {

			// Eine Taste wurde gedrueckt. Bei den Modifier Tasten wie SHIFT,
			// ALT, NUM_LOCK etc. wird nur der interne Zustand geaendert.
			// Durch die Rueckgabe der invaliden Taste wird angezeigt, dass
			// die Tastatureingabe noch nicht abgeschlossen ist. Bei den
			// anderen Tasten werden ASCII und Scancode eingetragen und eine
			// valide Taste fuer eine erfolgreiche Tastaturabfrage zurueck-
			// gegeben, obwohl eigentlich noch der Break-code der Taste fehlt.

			switch (code) {
				case 42:
				case 54:
					gather.SHIFT (true);
					break;
				case 56:
					if (prefix == prefix1)
						gather.alt_right (true);
					else
						gather.alt_left (true);
					break;
				case 29:
					if (prefix == prefix1)
						gather.ctrl_right (true);
					else
						gather.ctrl_left (true);
					break;
				case 58:
					gather.caps_lock (!gather.caps_lock ());
					keyctrl->set_led (Keyboard_Controller::led_caps_lock, gather.caps_lock());
					break;
				case 70:
					gather.scroll_lock (!gather.scroll_lock ());
					keyctrl->set_led (Keyboard_Controller::led_scroll_lock, gather.scroll_lock());
					break;
				case 69: // Numlock oder Pause ?
					if (gather.ctrl_left ()) { // Pause Taste
						// Auf alten Tastaturen konnte die Pause-Funktion wohl
						// nur ueber Ctrl+NumLock erreicht werden. Moderne
						// MF-II Tastaturen senden daher diese Codekombination,
						// wenn Pause gemeint ist. Die Pause Taste liefert zwar
						// normalerweise keinen ASCII Code, aber Nachgucken
						// schadet auch nicht. In jedem Fall ist die Taste nun
						// komplett.
						get_ascii_code (code);
					} else { // NumLock
						gather.num_lock (!gather.num_lock());
						keyctrl->set_led (Keyboard_Controller::led_num_lock, gather.num_lock ());
					}
					break;
				case 91: // linke Windows-Taste
				case 92: // rechte Windows-Taste
				case 93: // Menu-Taste
					// ignorieren
					break;
				default: // alle anderen Tasten
					// ASCII Codes aus den entsprechenden Tabellen auslesen, fertig.
					get_ascii_code (code);
			}
		}

		// Ein Prefix gilt immer nur fuer den unmittelbar nachfolgenden Code.
		// Also ist es jetzt abgehandelt.
		prefix = 0;
	}

	// Rückgabe der Taste
	return gather;
}

