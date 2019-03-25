#include "user/shell/shell.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"
#include "syscall/guarded_bell.h"
#include "user/time/rtc.h"
#include "machine/cgascr.h"

static CGA_Screen::Pixel *base = CGA_Screen::CGA_BASE;
static CGA_Screen::Pixel *backup_cga;
static int backup_out_x;
static int backup_out_y;
static int backup_dbg_x;
static int backup_dbg_y;

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
    dbg.getpos(backup_dbg_x, backup_dbg_y);
    screen_backup(out);
    screen_backup(dbg);
}

void Shell::restore() {
    out.setpos(backup_out_x, backup_out_y);
    dbg.setpos(backup_dbg_x, backup_dbg_y);
    screen_restore(out);
    screen_restore(dbg);

    delete backup_cga;
    backup_cga = nullptr;
}

void Shell::process_input(String *s) {
    s->remove_lf();

    if (streq(*s, "test")) {
        out << COLOR_GREEN << "success :)" << COLOR_RESET << endl;
    } else if (streq(*s, "yes")) {
        out << COLOR_YELLOW << "no" << COLOR_RESET << endl;
    } else if (streq(*s, "time") || streq(*s, "date")) {
        out << rtc.get_time() << endl;
    } else if (streq(*s, "cpu0")) {
        dout_CPU0 << "sup bitch" << endl;
    } else if (streq(*s, "cpu1")) {
        dout_CPU1 << "sup bitch" << endl;
    } else if (streq(*s, "cpu2")) {
        dout_CPU2 << "sup bitch" << endl;
    } else if (streq(*s, "cpu3")) {
        dout_CPU3 << "sup bitch" << endl;
    } else if (streq(*s, "reboot") || streq(*s, "restart")) {
        keyboard.reboot();
    } else if (streq(*s, "nullptr")) {
        out << "nullptr: " << (void *) nullptr << endl;
    } else if (streq(*s, "hex")) {
        out << hex << -1 << dec << endl;
    } else if (streq(s->to(5), "strtol")) {
        bool error;
        out << strtol(s->from(6), &error) << (error ? " (error!)" : "") << endl;
    }
}

void Shell::start() {
    backup();
    out.reset();
    dbg.reset();
    out << "Welcome " << (char) 1 << endl;
    dbg << "Welcome (DBG)" << endl;

    for (;;) {
        out << "$ " << flush;

        String s;
        keyboard >> s;
        if (s.is_empty()) {
            restore();
            return;
        }

        process_input(&s);
    }
}
