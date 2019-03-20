#include "user/shell/shell.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"
#include "syscall/guarded_bell.h"
#include "user/time/rtc.h"

static uint8_t backup[CGA_Screen::ROWS * CGA_Screen::COLUMNS * 2];
static int     backup_x;
static int     backup_y;

void Shell::screen_backup(CGA_Stream& str) {
    uint8_t *base = (uint8_t *) 0xb8000;
    for (int x = str.from_col; x < str.to_col; x++) {
        for (int y = str.from_row; y < str.to_row; y++) {
            backup[(y * str.COLUMNS + x) * 2]     = base[(y * str.COLUMNS + x) * 2];
            backup[(y * str.COLUMNS + x) * 2 + 1] = base[(y * str.COLUMNS + x) * 2 + 1];
        }
    }
}

void Shell::screen_backup() {
    stream.getpos(backup_x, backup_y);
    screen_backup(stream);
}

void Shell::screen_restore(CGA_Stream& str) {
    uint8_t *base = (uint8_t *) 0xb8000;
    for (int x = str.from_col; x < str.to_col; x++) {
        for (int y = str.from_row; y < str.to_row; y++) {
            base[(y * str.COLUMNS + x) * 2]     = backup[(y * str.COLUMNS + x) * 2];
            base[(y * str.COLUMNS + x) * 2 + 1] = backup[(y * str.COLUMNS + x) * 2 + 1];
        }
    }
}

void Shell::screen_restore() {
    stream.setpos(backup_x, backup_y);
    screen_restore(stream);
}

String *Shell::read(size_t count) {
    String *s = new String;

    for (size_t i = 0; i < count; i++) {
        Key k = keyboard.getkey();

        if (k.ctrl() && (k.ascii() == 'd' || k.ascii() == 'D')) {
            //dbg << "ctrl+d " << flush;
            break;
        }

        s->append(k.ascii());
        stream << k.ascii() << flush;

        if (k.ascii() == '\n') {
            break;
        }
    }

    return s;
}

void Shell::process_input(String *s) {
    s->remove_lf();
    dbg << "len: " << s->length() << ' ' << flush;

    if (streq(*s, "test")) {
        stream << "success :)" << endl;
    } else if (streq(*s, "yes")) {
        stream << "no" << endl;
    } else if (streq(*s, "time") || streq(*s, "date")) {
        stream << rtc.get_time() << endl;
    } else if (streq(*s, "cpu0")) {
        dout_CPU0 << "sup bitch" << endl;
    } else if (streq(*s, "cpu1")) {
        dout_CPU1 << "sup bitch" << endl;
    } else if (streq(*s, "cpu2")) {
        dout_CPU2 << "sup bitch" << endl;
    } else if (streq(*s, "cpu3")) {
        dout_CPU3 << "sup bitch" << endl;
    } else if (streq(*s, "status")) {
        dout_status << "sup bitch" << flush;
    } else if (streq(*s, "clock")) {
        dout_clock << "sup bitch" << flush;
    } else if (streq(*s, "reboot")) {
        keyboard.reboot();
    }
}

void Shell::start() {
    screen_backup();
    screen_backup(dbg);
    stream.reset();
    dbg.reset();
    stream << "Welcome " << ((char) 1) << endl;
    dbg    << "Welcome (DBG)" << endl;

    for (;;) {
        String *s = read(STRING_MAX_LENGTH);
        if (s->is_empty()) {
            delete s;

            stream << "Goodbye!" << endl;
            Guarded_Bell::sleep(2000);
            screen_restore();
            screen_restore(dbg);
            return;
        }

        process_input(s);
        delete s;
    }
}
