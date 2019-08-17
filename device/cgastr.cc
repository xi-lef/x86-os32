// vim: set et ts=4 sw=4:

#include "device/cgastr.h"
#include "debug/output.h"

// global constructor order is important here!
DECL_COLOR(BLACK);
DECL_COLOR(BLUE);
DECL_COLOR(GREEN);
DECL_COLOR(CYAN);
DECL_COLOR(RED);
DECL_COLOR(MAGENTA);
DECL_COLOR(BROWN);
DECL_COLOR(LIGHT_GREY);
DECL_COLOR(DARK_GREY);
DECL_COLOR(LIGHT_BLUE);
DECL_COLOR(LIGHT_GREEN);
DECL_COLOR(LIGHT_CYAN);
DECL_COLOR(LIGHT_RED);
DECL_COLOR(LIGHT_MAGENTA);
DECL_COLOR(YELLOW);
DECL_COLOR(WHITE);
DECL_COLOR(BLACK, LIGHT_GREY);

CGA_Screen::Attribute COLOR_RESET(CGA_Screen::WHITE, CGA_Screen::WHITE, true); // invalid color

CGA_Stream kout(0,  79,  0, 14, true);
Guarded_Mutex kout_mutex;

CGA_Stream dout_CPU0(0,  39, 15, 19, false, COLOR_LIGHT_BLUE);
CGA_Stream dout_CPU1(40, 79, 15, 19, false, COLOR_LIGHT_GREEN);
CGA_Stream dout_CPU2(0,  39, 20, 23, false, COLOR_LIGHT_RED);
CGA_Stream dout_CPU3(40, 79, 20, 23, false, COLOR_LIGHT_MAGENTA);

CGA_Stream dout_status(0, 79 - TIME_DISPLAY_LENGTH - 1, 24, 24, false,
                       COLOR_BLACK_LIGHT_GREY);
CGA_Stream dout_clock(79 - TIME_DISPLAY_LENGTH, 79, 24, 24, false,
                      COLOR_BLACK_LIGHT_GREY);

void CGA_Stream::flush() {
    print(buffer, pos, attrib);
    pos = 0;
}

void CGA_Stream::reset(char c) {
    CGA_Screen::reset(c, attrib);
}

void CGA_Stream::backspace(String *str, size_t pos) {
    if (str) {
        if (str->empty()) {
            return;
        }
        str->erase(pos, 1);
    }

    int x, y;
    getpos(x, y);
    if (x == from_col) {
        if (y != from_row) {
            y--;
        }
        x = to_col;
    } else {
        x--;
    }
    show(x, y, ' ');
    setpos(x, y);
}

O_Stream& CGA_Stream::operator <<(Attribute& attr) {
    flush();
    attrib = (&attr == &COLOR_RESET) ? orig_attrib : attr;
    return *this;
}

O_Stream& CGA_Stream::operator <<(O_Stream& (*f) (CGA_Stream&)) {
    return f(*this);
}

O_Stream& backspace(CGA_Stream& os) {
    os.backspace();
    return os;
}
