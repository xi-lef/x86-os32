// vim: set et ts=4 sw=4:

#include "device/cgastr.h"

CGA_Stream kout     ( 0, 79,  0, 14, true);
CGA_Stream dout_CPU0( 0, 39, 15, 19, false);
CGA_Stream dout_CPU1(40, 79, 15, 19, false);
CGA_Stream dout_CPU2( 0, 39, 20, 24, false);
CGA_Stream dout_CPU3(40, 79, 20, 24, false);

void CGA_Stream::flush() {
    print(buffer, pos);

    // for better debugging
    for (int i = 0; i < 80; i++) {
        buffer[i] = 0;
    }
    pos = 0;
}
