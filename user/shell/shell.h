#pragma once

#include "device/cgastr.h"
#include "user/string/string.h"

class Shell {
private:
    CGA_Stream &out;
    CGA_Stream &dbg;

    void screen_backup(CGA_Stream& str);
    void screen_restore(CGA_Stream& str);

    void backup();
    void restore();

public:
    Shell(CGA_Stream &out, CGA_Stream &dbg) : out(out), dbg(dbg) {}

    void process_input(String *s);

    void start();
};
