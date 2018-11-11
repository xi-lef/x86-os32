#include "user/shell/shell.h"
#include "user/io/string.h"
#include "debug/output.h"

static String s;
static uint8_t backup[CGA_Screen::ROWS * CGA_Screen::COLUMNS * 2];
static int     backup_x;
static int     backup_y;

Shell::Shell(CGA_Stream &str, Keyboard_Controller &kb) : stream(str), keyboard(kb) {}

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

void Shell::start() {
    screen_backup();
    screen_backup(DBG);
    stream.reset();
    DBG.reset();
    stream << "Welcome " << ((char) 1) << endl;
    DBG    << "Welcome (DBG)" << endl;

    for (;;) {
        s.empty();
        for (int i = 0; i < STRING_MAX_LENGTH; i++) {
            //DBG << i << endl;
            Key k = keyboard.key_hit();

            if (k.ctrl() && (k.ascii() == 'd' || k.ascii() == 'D')) {
                // not at the beginning of a line -> process the input thus far ... TODO fix
                if (i != 0) {
                    break;
                }
                // ... otherwise, exit the shell
                if (!k.SHIFT()) {
                    stream << "Goodbye!" << endl;
                    sleep(2);
                }
                screen_restore();
                screen_restore(DBG);
                return;
            }

            if (k.valid()) {
                s.append(k.ascii());
                stream << k.ascii() << flush;
            } else { // breakcode (etc)
                i--;
                continue;
            }

            if (k.ascii() == '\n') {
                break;
            }
        }

        s.remove_nl();
        if (streq(s, "test")) {
            stream << "success :)" << endl;
        } else if (streq(s, "yes")) {
            stream << "no" << endl;
        } else if (streq(s, "time") || streq(s, "date")) {
            stream << get_time() << endl;
        } else if (streq(s, "cpu0")) {
            dout_CPU0 << "sup bitch" << endl;
        } else if (streq(s, "cpu1")) {
            dout_CPU1 << "sup bitch" << endl;
        } else if (streq(s, "cpu2")) {
            dout_CPU2 << "sup bitch" << endl;
        } else if (streq(s, "cpu3")) {
            dout_CPU3 << "sup bitch" << endl;
        } else if (streq(s, "status")) {
            //dout_status << "sup bitch";
        } else if (streq(s, "clock")) {
            //dout_clock << "sup bitch";
        } else {
            //DBG << s << " didnt match anything!" << endl;
        }
    }
}
