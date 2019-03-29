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

    void reset(char c = ' ') {
        CGA_Screen::reset(c, attrib);
    }

    using O_Stream::operator<<; // necessary because c++
    O_Stream& operator <<(Attribute& attr) override;
};

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define DECL_EXT_COLOR1(fg)            extern CGA_Screen::Attribute COLOR_##fg
#define DECL_EXT_COLOR2(fg, bg)        extern CGA_Screen::Attribute COLOR_##fg##_##bg
#define DECL_EXT_COLOR3(fg, bg, blink) extern CGA_Screen::Attribute COLOR_##fg##_##bg##blink
#define DECL_EXT_COLOR(...) GET_MACRO(__VA_ARGS__, DECL_EXT_COLOR3, DECL_EXT_COLOR2, DECL_EXT_COLOR1)(__VA_ARGS__)

#define DECL_COLOR1(fg)            CGA_Screen::Attribute COLOR_##fg(CGA_Screen::fg)
#define DECL_COLOR2(fg, bg)        CGA_Screen::Attribute COLOR_##fg##_##bg(CGA_Screen::fg, CGA_Screen::bg)
#define DECL_COLOR3(fg, bg, blink) CGA_Screen::Attribute COLOR_##fg##_##bg##blink(CGA_Screen::fg, CGA_Screen::bg, blink)
#define DECL_COLOR(...) GET_MACRO(__VA_ARGS__, DECL_COLOR3, DECL_COLOR2, DECL_COLOR1)(__VA_ARGS__)

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
