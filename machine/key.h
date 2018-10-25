// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält Klasse Key
 */

#pragma once

/*! \brief Abstraktion für eine Taste bestehend aus ASCII-Code, Scancode und
 *  Modifierbits.
 */
class Key
{
private:
	// Kopieren erlaubt!
	unsigned char asc;
	unsigned char scan;
	unsigned char modi;

	// Bit-Masken fuer die Modifier-Tasten
	struct mbit {
		enum {
			SHIFT       = 1,
			alt_left    = 2,
			alt_right   = 4,
			ctrl_left   = 8,
			ctrl_right  = 16,
			caps_lock   = 32,
			num_lock    = 64,
			scroll_lock = 128
		};
	};

public:
	/*! \brief Default-Konstruktor: setzt ASCII, Scancode und Modifier auf 0
	 *  und bezeichnet so einen ungültigen Tastencode
	 *
	 */
	Key () : asc (0), scan (0), modi (0) {}

	/*! \brief Mit Scancode = 0 werden ungültige Tasten gekennzeichnet.
	 *  \return Gibt an, ob der Tastencode gültig ist.
	 */
	bool valid ()      {
		return scan != 0;
	}

	/*! \brief Setzt den Scancode auf 0 und sorgt somit für einen
	 *  ungültigen Tastencode.
	 *
	 */
	void invalidate () {
		scan = 0;
	}

	// ASCII, SCANCODE: Setzen und Abfragen von Ascii und Scancode
	/*! \brief Setzt ASCII-Code
	 *  \param a ASCII-Code
	 */
	void ascii (unsigned char a) {
		asc = a;
	}
	/*! \brief Setzt Scancode
	 *  \param s Scancode
	 */
	void scancode (unsigned char s) {
		scan = s;
	}
	/*! \brief Abfragen des ASCII-Codes
	 *  \return ASCI-Code
	 */
	unsigned char ascii () {
		return asc;
	}
	/*! \brief Abfragen des Scancodes
	 *  \return Scancode
	 */
	unsigned char scancode () {
		return scan;
	}

	/*! \brief Setzt den SHIFT Modifier
	 *
	 */
	void SHIFT (bool pressed) {
		modi = pressed ? modi | mbit::SHIFT : modi & ~mbit::SHIFT;
	}
	/*! \brief Setzt den alt_left Modifier
	 *
	 */
	void alt_left (bool pressed) {
		modi = pressed ? modi | mbit::alt_left : modi & ~mbit::alt_left;
	}
	/*! \brief Setzt den alt_right Modifier
	 *
	 */
	void alt_right (bool pressed) {
		modi = pressed ? modi | mbit::alt_right : modi & ~mbit::alt_right;
	}
	/*! \brief Setzt den ctrl_left Modifier
	 *
	 */
	void ctrl_left (bool pressed) {
		modi = pressed ? modi | mbit::ctrl_left : modi & ~mbit::ctrl_left;
	}
	/*! \brief Setzt den ctrl_right Modifier
	 *
	 */
	void ctrl_right (bool pressed) {
		modi = pressed ? modi | mbit::ctrl_right : modi & ~mbit::ctrl_right;
	}
	/*! \brief Setzt den caps_lock Modifier
	 *
	 */
	void caps_lock (bool pressed) {
		modi = pressed ? modi | mbit::caps_lock : modi & ~mbit::caps_lock;
	}
	/*! \brief Setzt den num_lock Modifier
	 *
	 */
	void num_lock (bool pressed) {
		modi = pressed ? modi | mbit::num_lock : modi & ~mbit::num_lock;
	}
	/*! \brief Setzt den scroll_lock Modifier
	 *
	 */
	void scroll_lock (bool pressed) {
		modi = pressed ? modi | mbit::scroll_lock : modi & ~mbit::scroll_lock;
	}

	/*! \brief Zeigt an, ob Modifier SHIFT vorhanden ist
	 *
	 */
	bool SHIFT ()       {
		return modi & mbit::SHIFT;
	}
	/*! \brief Zeigt an, ob Modifier alt_left vorhanden ist
	 *
	 */
	bool alt_left ()    {
		return modi & mbit::alt_left;
	}
	/*! \brief Zeigt an, ob Modifier alt_right vorhanden ist
	 *
	 */
	bool alt_right ()   {
		return modi & mbit::alt_right;
	}
	/*! \brief Zeigt an, ob Modifier ctrl_left vorhanden ist
	 *
	 */
	bool ctrl_left ()   {
		return modi & mbit::ctrl_left;
	}
	/*! \brief Zeigt an, ob Modifier ctrl_right vorhanden ist
	 *
	 */
	bool ctrl_right ()  {
		return modi & mbit::ctrl_right;
	}
	/*! \brief Zeigt an, ob Modifier caps_lock vorhanden ist
	 *
	 */
	bool caps_lock ()   {
		return modi & mbit::caps_lock;
	}
	/*! \brief Zeigt an, ob Modifier num_lock vorhanden ist
	 *
	 */
	bool num_lock ()    {
		return modi & mbit::num_lock;
	}
	/*! \brief Zeigt an, ob Modifier scroll_lock vorhanden ist
	 *
	 */
	bool scroll_lock () {
		return modi & mbit::scroll_lock;
	}
	/*! \brief Zeigt an, ob Modifier alt vorhanden ist
	 *
	 */
	bool alt ()         {
		return alt_left ()  | alt_right ();
	}
	/*! \brief Zeigt an, ob Modifier ctrl vorhanden ist
	 *
	 */
	bool ctrl ()        {
		return ctrl_left () | ctrl_right ();
	}

	/*! \brief Liefert ASCII-Wert
	 *
	 */
	operator char ()    {
		return (char) asc;
	}

	/*! \brief Scan-Codes einiger spezieller Tasten
	 *
	 */
	struct scan {
		enum {
			f1 = 0x3b, del = 0x53, up=72, down=80, left=75, right=77,
			div = 8
		};
	};
};

