#include "user/shell/shell.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"
#include "syscall/guarded_bell.h"
#include "user/time/rtc.h"
#include "machine/cgascr.h"
#include "object/queue.h"

static CGA_Screen::Pixel *base = CGA_Screen::CGA_BASE;
static CGA_Screen::Pixel *backup_cga;
static int backup_out_x, backup_out_y;

static String prompt("$ ");

void Shell::screen_backup(CGA_Stream& str) {
    for (int x = str.from_col; x <= str.to_col; x++) {
        for (int y = str.from_row; y <= str.to_row; y++) {
            backup_cga[y * str.COLUMNS + x] = base[y * str.COLUMNS + x];
        }
    }
}

void Shell::screen_restore(CGA_Stream& str) {
    for (int x = str.from_col; x <= str.to_col; x++) {
        for (int y = str.from_row; y <= str.to_row; y++) {
            base[y * str.COLUMNS + x] = backup_cga[y * str.COLUMNS + x];
        }
    }
}

void Shell::backup() {
    if (!backup_cga) {
        backup_cga = new CGA_Screen::Pixel[CGA_Screen::ROWS * CGA_Screen::COLUMNS];
    }

    out.getpos(backup_out_x, backup_out_y);
    screen_backup(out);
}

void Shell::restore() {
    out.setpos(backup_out_x, backup_out_y);
    screen_restore(out);

    delete backup_cga;
    backup_cga = nullptr;
}

void Shell::history_destroy() {
    for (History_Entry *e = history_head; e;) {
        delete e->str;
        History_Entry *tmp = e;
        e = e->next;
        delete tmp;
    }
}

void Shell::history_add(String *s) {
    // dont add the same command twice
    if (*s == *history_tail->str) {
        return;
    }

    History_Entry *new_entry = new History_Entry;
    new_entry->str = s;
    if (!history_head) {
        history_head = new_entry;
        history_tail = new_entry;
        new_entry->prev = nullptr;
        new_entry->next = nullptr;
    } else {
        history_tail->next = new_entry;
        new_entry->prev = history_tail;
        new_entry->next = nullptr;
        history_tail = new_entry;
    }
}

size_t Shell::read(String *s, size_t count) {
    History_Entry *cur = nullptr; // for browsing shell history
    int x_start, y_start;
    out.getpos(x_start, y_start);

    s->empty();
    while (s->length() < count) {
        Key k = keyboard.getkey();

        // ctrl + d makes read terminate
        if (k.ctrl() && (k.ascii() == 'd' || k.ascii() == 'D')) {
            break;
        }

        // catch backspace
        if (k.ascii() == '\b') {
            Keyboard::handle_backspace(s, out);
            continue;
        }

        // catch arrow keys (for shell history)
        bool hist = false;
        if (k.scancode() == Key::scan::up) {
            if (!cur) {
                cur = history_tail;
                hist = true;
            } else if (!cur->prev) {
                continue;
            } else {
                cur = cur->prev;
                hist = true;
            }
        } else if (k.scancode() == Key::scan::down) {
            if (!cur) {
                continue;
            } else {
                cur = cur->next;
                hist = true;
            }
        }
        if (hist) {
            // "erase" currently displayed string
            int x, y;
            out.getpos(x, y);
            while (x != x_start || y != y_start) {
                if (--x < out.from_col) {
                    y--;
                    x = out.to_col;
                }
                out.show(x, y, ' ');
            }
            out.setpos(x, y);

            // print new string
            *s = *cur->str;
            out << *s << flush;
            continue;
        }

        s->append(k.ascii());
        out << k.ascii() << flush;

        if (k.ascii() == '\n') {
            break;
        }
    }

    return s->length();
}

void Shell::process_input(String *s) {
    s->remove_lf();
    if (!s->is_empty()) {
        history_add(s);
    }

    String cmd = s->tok(" ");
    if (streq(cmd, "test")) {
        out << COLOR_GREEN << "success :)" << COLOR_RESET << endl;
    } else if (streq(cmd, "yes")) {
        out << COLOR_YELLOW << "no" << COLOR_RESET << endl;
    } else if (streq(cmd, "time") || streq(cmd, "date")) {
        out << rtc.get_time() << endl;
    } else if (streq(cmd, "cpu0")) {
        dout_CPU0 << "sup bitch" << endl;
    } else if (streq(cmd, "cpu1")) {
        dout_CPU1 << "sup bitch" << endl;
    } else if (streq(cmd, "cpu2")) {
        dout_CPU2 << "sup bitch" << endl;
    } else if (streq(cmd, "cpu3")) {
        dout_CPU3 << "sup bitch" << endl;
    } else if (streq(cmd, "reboot") || streq(cmd, "restart")) {
        keyboard.reboot();
    } else if (streq(cmd, "nullptr")) {
        out << "nullptr: " << (void *) nullptr << endl;
    } else if (streq(cmd, "hex")) {
        out << hex << -1 << dec << endl;
    } else if (streq(cmd, "strtol")) {
        String num = s->tok(" ");
        String tmp = s->tok(" ");
        bool error;
        int base = strtol(tmp, &error);
        if (error) {
            out << "base invalid, using 0" << endl;
            base = 0;
        }
        out << strtol(num, &error, base) << (error ? " (error!)" : "") << endl;
    } else if (streq(cmd, "strtok")) {
        String arg;
        while (!(arg = s->tok(" ")).is_empty()) {
            out << arg << endl;
        }
    } else if (streq(cmd, "insert")) {
        String str = s->tok(" ");
        String ins = s->tok(" ");
        long pos = strtol(s->tok(" "));
        out << "inserting " << ins << " into " << str << " at " << pos << ":" << endl
            << str.insert(pos, ins) << endl;
    } else {
        out << cmd << ": command not found" << endl;
    }
}

void Shell::start() {
    backup();
    out.reset();
    out << "Welcome " << (char) 1 << endl;

    for (;;) {
        out << prompt << flush;

        String *s = new String;
        if (!read(s, s->MAX_LENGTH)) {
            break;
        }

        process_input(s);
    }

    history_destroy();
    restore();
}
