#pragma once

#include "device/cgastr.h"
#include "user/string/string.h"
#include "utils/heap.h"
#include "machine/key.h"

class Shell {
private:
    CGA_Stream &out;

    void screen_backup(CGA_Stream& str);
    void screen_restore(CGA_Stream& str);

    void backup();
    void restore();

    struct History_Entry {
        String        *str;
        History_Entry *prev;
        History_Entry *next;
    };

    History_Entry *history_head;
    History_Entry *history_tail;

    void history_destroy();
    void history_add(String *s);

public:
    Shell(CGA_Stream &out) : out(out), history_head(nullptr), history_tail(nullptr) {}

    void perror(String cmd, char *error) const;

    size_t read(String *str, size_t count);
    void process_input(String *str);

    void start();
};
