#pragma once

#include "device/cgastr.h"
#include "user/io/string.h"

class Shell {
private:
    CGA_Stream &stream;
    CGA_Stream &dbg;

    void screen_backup();
    void screen_backup(CGA_Stream& str);
    void screen_restore();
    void screen_restore(CGA_Stream& str);

public:
    Shell(CGA_Stream &str, CGA_Stream &dbg) : stream(str), dbg(dbg) {}

    String *read(size_t count);

    void process_input(String *s);

    void start();
};
