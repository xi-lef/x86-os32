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
    for (History_Entry *e = history_tail; e;) {
        delete e->str;
        History_Entry *tmp = e;
        e = e->prev;
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
    if (!history_tail) {
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

void Shell::perror(String cmd, char *error) const {
    out << cmd << ": " << error << endl;
}

size_t Shell::read(String *str, size_t count) {
    History_Entry *cur = nullptr; // for browsing shell history
    int x_start, y_start; // start position; to take shell prompt into account
    out.getpos(x_start, y_start);

    str->clear();
    size_t cursor = 0;

    while (str->length() < count) {
        Key k = keyboard.getkey();

        // ctrl + d makes read terminate
        if (k.ctrl() && (k.ascii() == 'd' || k.ascii() == 'D')) {
            break;
        }

        // catch newline
        if (k.ascii() == '\n') {
            str->append('\n');
            out << endl;
            break;
        }

        /// catch "special" keys
        bool hist = false;
        bool curs = false;

        switch (k.scancode()) {
        // catch arrow keys for shell history
        case Key::scan::up: {
            hist = true;
            if (!cur) {
                cur = history_tail;
            } else if (!cur->prev) {
                continue;
            } else {
                cur = cur->prev;
            }
        } break;
        case Key::scan::down: {
            hist = true;
            if (!cur) {
                continue;
            } else {
                cur = cur->next;
            }
        } break;

        // catch arrow keys for changing cursor
        case Key::scan::left: {
            curs = true;
            if (cursor == 0) {
                continue;
            }
            cursor--;
        } break;
        case Key::scan::right: {
            curs = true;
            if (cursor >= str->length()) {
                continue;
            }
            cursor++;
        } break;
        } // switch (k.scancode())

        // catch backspace
        if (k.ascii() == '\b') {
            out.backspace(str, cursor - 1);
            if (cursor != 0) {
                cursor--;
            }
        } else if (!hist && !curs) {
            str->insert(cursor, k.ascii());
            cursor++;
        }

        // TODO detect scrollup (when at bottom of out).
        // in that case y_start (and x_start ?) will not be correct
        /*if (y_start == out.to_row && x_start + str->length() >= (unsigned int) out.to_col) {
            y_start -= (x_start + str->length()) / out.width;
        }*/
        size_t num_rows = (x_start + str->length() - 1) / (out.width - 1);
        if (num_rows > (unsigned int) out.to_row - y_start) {
            DBG << "num_rows(" << num_rows << ") " << flush;
            //y_start -= num_rows;
            y_start--;
            //out.move_up_one_line();
        }

        // erase currently displayed string if the string changed
        if (!curs) {
            out.setpos(x_start, y_start);
            // "+ 1" in case "k" is a backspace in the middle of the string
            for (size_t i = 0; i < str->length() + 1; i++) {
                out << ' ';
            }
            out << flush;
        }

        // TODO scrollup here?

        // change str if a different command was selected from shell history
        if (hist) {
            *str = *cur->str;
            cursor = str->length();
        }

        // print new string
        out.setpos(x_start, y_start);
        out << *str << flush;
        out.setpos(Math::min((unsigned int) out.to_col, (x_start + cursor) % out.width),
                   Math::min((unsigned int) out.to_row, (x_start + cursor) / out.width + y_start));

        //DBG << "c: " << cursor << ' ' << flush;
    }

    return str->length();
}

void Shell::process_input(String *str) {
    str->remove_lf();
    if (!str->empty()) {
        history_add(str);
    }

    String cmd = str->tok(" ");
    if (streq(cmd, "test")) {
        out << COLOR_GREEN << "success :)" << COLOR_RESET << endl;
        /*String s("hallo welt");
        out << s.substr(4, 5) << endl;
        out << s.erase(2, 4) << endl;
        String a = s.tok(" ");
        String b = s.tok(" ");
        out << a << endl;
        out << b << endl;*/
    } else if (streq(cmd, "yes")) {
        out << COLOR_YELLOW << "no" << COLOR_RESET << endl;
    } else if (streq(cmd, "time") || streq(cmd, "date")) {
        out << rtc << endl;
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
        String num = str->tok(" ");
        String tmp = str->tok(" ");
        bool error;
        int base = strtol(tmp, &error);
        if (!tmp.empty() && error) {
            out << "base invalid, using autodetect" << endl;
            base = 0;
        }

        out << strtol(num, &error, base) << (error ? " (error!)" : "") << endl;
    } else if (streq(cmd, "strtok")) {
        String arg;
        while (!(arg = str->tok(" ")).empty()) {
            out << arg << endl;
        }
    } else if (streq(cmd, "insert")) {
        String s     = str->tok(" ");
        String ins   = str->tok(" ");
        String pos_s = str->tok(" ");
        if (s.empty() || ins.empty()) {
            perror(cmd, "usage: insert <str> <insert_str> <pos>");
            return;
        }

        long pos = strtol(pos_s);
        out << "inserting " << ins << " into " << s << " at " << pos << ":" << endl
            << s.insert(pos, ins) << endl;
    } else if (streq(cmd, "settime")) {
        String hour_s   = str->tok(" :/-,.");
        String minute_s = str->tok(" :/-,.");
        String second_s = str->tok(" :/-,.");
        if (hour_s.empty() || minute_s.empty() || second_s.empty()) {
            perror(cmd, "usage: settime <hour> <minute> <second>");
            return;
        }

        rtc.set_hour(strtol(hour_s));
        rtc.set_minute(strtol(minute_s));
        rtc.set_second(strtol(second_s));
        rtc.update_time();
    } else if (streq(cmd, "settimezone")) {
        String zone_s = str->tok(" ");
        if (zone_s.empty()) {
            perror(cmd, "usage: settimezone <timezone> (only positive values!)");
        }

        rtc.set_timezone(strtol(zone_s));
        rtc.update_time();
    } else if (streq(cmd, "setdate")) {
        String day_s    = str->tok(" :/-,.");
        String month_s  = str->tok(" :/-,.");
        String year_s   = str->tok(" :/-,.");
        String weekday_s = str->tok(" :/-,.");
        if (day_s.empty() || month_s.empty() || year_s.empty()) {
            perror(cmd, "usage: setdate <day> <month> <year> [<numeric weekday>]");
            return;
        }

        rtc.set_day(strtol(day_s));
        rtc.set_month(strtol(month_s));
        rtc.set_real_year(strtol(year_s));
        if (!weekday_s.empty()) {
            rtc.set_weekday(strtol(weekday_s));
        }
        rtc.update_time();
    } else {
        perror(cmd, "command not found");
    }
}

void Shell::start() {
    backup();
    out.reset();
    out << "Welcome " << (char) 1 << endl;

    for (;;) {
        out << prompt << flush;

        String *str = new String;
        if (!read(str, 4096)) {
            break;
        }

        process_input(str);
    }

    history_destroy();
    restore();
}
