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

        History_Entry() : str(nullptr), prev(nullptr), next(nullptr) {}
        History_Entry(String *s) : prev(nullptr), next(nullptr) {
            //str = new String(*s); // TODO hm
            str = s;
        }
        ~History_Entry() {
            delete str;
        }
    };

    History_Entry *history_tail;

    void history_destroy();
    void history_add(String *str);

public:
    Shell(CGA_Stream &out) : out(out), history_tail(nullptr) {}

    void perror(String cmd, char *error) const;

    size_t read(String *str, size_t count);
    void process_input(String *str);

    void start();
};
