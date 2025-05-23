// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse CGA_Stream
 */

/*! \defgroup io I/O Subsystem
	\brief Das Eingabe-/Ausgabe Subsystem

*/

#pragma once

#include "object/o_stream.h"
#include "machine/cgascr.h"
#include "syscall/guarded_mutex.h"
#include "user/string/string.h"

/*! \brief Darstellung verschiedener Datentypen auf dem Bildschrim
 *  \ingroup io
 *
 *
 *  Die Klasse CGA_Stream ermöglicht die Ausgabe verschiedener Datentypen als
 *  Zeichenketten auf dem CGA Bildschirm eines PCs. Dazu braucht CGA_Stream nur
 *  von den Klassen O_Stream und CGA_Screen abgeleitet und endlich die Methode
 *  flush() implementiert werden. Für weitergehende Formatierung oder spezielle
 *  Effekte stehen die Methoden der Klasse CGA_Screen zur Verfügung.
 */
class CGA_Stream : public O_Stream, public CGA_Screen {
	// Verhindere Kopien und Zuweisungen
	CGA_Stream(const CGA_Stream&)            = delete;
	CGA_Stream& operator=(const CGA_Stream&) = delete;

private:
    const Attribute orig_attrib;
    Attribute attrib;

public:
    /// \copydoc CGA_Screen::CGA_Screen(int, int, int, int, bool)
	CGA_Stream(int from_col, int to_col, int from_row, int to_row,
        bool use_cursor = false, Attribute attr = Attribute()) : CGA_Screen(from_col, to_col,
        from_row, to_row, use_cursor), orig_attrib(attr), attrib(attr) {
            CGA_Screen::reset(' ', attrib);
        }

	/*! \brief Methode zur Ausgabe des Pufferinhalts der Basisklasse Stringbuffer.
	 *
	 *  Die Methode wird implizit aufgerufen, sobald der Puffer voll ist,
	 *  kann aber auch explizit verwendet werden, um eine Ausgabe zu erzwingen.
	 *
	 *  \todo Methode implementieren
	 */
	virtual void flush() override;

    void reset(char c = ' ');

    // "pos" is the position in "str" that will be erased by the backspace.
    void backspace(String *str = nullptr, size_t pos = String::npos);

    using O_Stream::operator<<; // necessary because c++
    O_Stream& operator <<(Attribute& attr) override;
    O_Stream& operator <<(O_Stream& (*f) (CGA_Stream&));
};

O_Stream& backspace(CGA_Stream& os);

DECL_EXT_COLOR(BLACK);
DECL_EXT_COLOR(BLUE);
DECL_EXT_COLOR(GREEN);
DECL_EXT_COLOR(CYAN);
DECL_EXT_COLOR(RED);
DECL_EXT_COLOR(MAGENTA);
DECL_EXT_COLOR(BROWN);
DECL_EXT_COLOR(LIGHT_GREY);
DECL_EXT_COLOR(DARK_GREY);
DECL_EXT_COLOR(LIGHT_BLUE);
DECL_EXT_COLOR(LIGHT_GREEN);
DECL_EXT_COLOR(LIGHT_CYAN);
DECL_EXT_COLOR(LIGHT_RED);
DECL_EXT_COLOR(LIGHT_MAGENTA);
DECL_EXT_COLOR(YELLOW);
DECL_EXT_COLOR(WHITE);
DECL_EXT_COLOR(BLACK, LIGHT_GREY);

extern CGA_Screen::Attribute COLOR_RESET; // invalid color

extern CGA_Stream kout;
extern Guarded_Mutex kout_mutex;
extern CGA_Stream dout_status;
extern CGA_Stream dout_clock;
