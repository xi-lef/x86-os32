// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält Klasse Key_Decoder
 */

#pragma once

#include "machine/key.h"

class Keyboard_Controller;

/*! \brief Dekodierer für die empfangenen Tastaturcontrollercodes
 *  \ingroup io
 *
 *
 *  Die Klasse Key_Decoder dient dazu, aus den Make- und Break-Codes
 *  den Scan- und ASCII Code der gedrückten Taste zu bestimmen.
 */
class Key_Decoder
{
	// Verhindere Kopien und Zuweisungen
	Key_Decoder(const Key_Decoder&)            = delete;
	Key_Decoder& operator=(const Key_Decoder&) = delete;
public:
	Key_Decoder(Keyboard_Controller * keyctrl) : keyctrl(keyctrl) {};

	/*! \brief Interpretiert die Make und Break-Codes der Tastatur und
	 *  liefert den ASCII Code, den Scancode und Informationen darüber,
	 *  welche zusätzlichen Tasten wie Shift und Ctrl gedrückt wurden.
	 *
	 *  \return gelesene Taste (invalid, falls das das Zeichen noch nicht
	 *  komplett ist und noch Make- oder Breakcodes fehlen)
	 */
	Key decode(unsigned char code);

private:
	Keyboard_Controller * const keyctrl;
	unsigned char prefix;
	Key gather;

	static unsigned char normal_tab[];
	static unsigned char shift_tab[];
	static unsigned char alt_tab[];
	static unsigned char asc_num_tab[];
	static unsigned char scan_num_tab[];

	// Konstanten fuer die Tastaturdekodierung
	enum { break_bit = 0x80, prefix1 = 0xe0, prefix2   = 0xe1 };

	/*! \brief ermittelt anhand von Tabellen aus dem Scancode und den
	 *  gesetzten Modifier-Bits den ASCII Code der Taste.
	 *
	 */
	void get_ascii_code (unsigned char code);
};

