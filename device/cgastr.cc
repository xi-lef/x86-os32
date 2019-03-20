// vim: set et ts=4 sw=4:

#include "device/cgastr.h"

CGA_Stream kout     ( 0, 79,  0, 14, true);
Guarded_Mutex kout_mutex;

CGA_Stream dout_CPU0( 0, 39, 15, 19, false);
CGA_Stream dout_CPU1(40, 79, 15, 19, false);
CGA_Stream dout_CPU2( 0, 39, 20, 23, false);
CGA_Stream dout_CPU3(40, 79, 20, 23, false);

CGA_Stream dout_status( 0, 79 - TIME_DISPLAY_LENGTH - 1, 24, 24, false,
        CGA_Screen::Attribute(CGA_Screen::BLACK, CGA_Screen::LIGHT_GREY));
CGA_Stream dout_clock (79 - TIME_DISPLAY_LENGTH, 79, 24, 24, false,
        CGA_Screen::Attribute(CGA_Screen::BLACK, CGA_Screen::LIGHT_GREY));

void CGA_Stream::flush() {
    print(buffer, pos, attrib);
    pos = 0;
}
