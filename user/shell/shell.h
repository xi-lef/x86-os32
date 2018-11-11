#pragma once

#include "device/cgastr.h"
#include "machine/keyctrl.h"

class Shell {
private:
    CGA_Stream &stream;
    Keyboard_Controller &keyboard;

    void screen_backup();
    void screen_backup(CGA_Stream& str);
    void screen_restore();
    void screen_restore(CGA_Stream& str);

public:
    Shell(CGA_Stream &str, Keyboard_Controller & kb);

    void start();
};
