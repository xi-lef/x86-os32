
#include "user/app2/kappl.h"
#include "syscall/guarded_semaphore.h"
#include "device/cgastr.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"
#include "utils/random.h"
#include "user/shell/shell.h"

void KeyboardApplication::action() {
    kout_mutex.lock();

    int x, y;
    kout.getpos(x, y);
    kout.setpos(40, 1);
    kout << "KB_App: " << flush;
    kout.setpos(x, y);

    kout_mutex.unlock();

    Random r(1337);
    int cur_speed = 0;
    int cur_delay = 0;

    // this writes in a 36 columns and 11 rows big area of kout
    for (int pos = 0; ; ++pos %= 36 * 11) {
        //char *test = new char[r.number() % (1024 * 900)];
        Key k = keyboard.getkey();
        //delete[] test;

        //DBG << int(k.scancode()) << endl;
        if (k.ctrl()) {
            if (k.ascii() == 't') {
                Shell shell(kout, DBG);
                kout_mutex.lock();
                shell.start();
                kout_mutex.unlock();
                continue;
            }
            switch (k.scancode()) {
            case Key::scan::up:
                keyboard.set_repeat_rate((cur_speed == 31) ? 31 : ++cur_speed, cur_delay);
                break;
            case Key::scan::down:
                keyboard.set_repeat_rate((cur_speed == 0)  ?  0 : --cur_speed, cur_delay);
                break;
            }
            continue;
        } else if (k.alt()) {
            switch (k.scancode()) {
            case Key::scan::up:
                keyboard.set_repeat_rate(cur_speed, (cur_delay == 3) ? 3 : ++cur_delay);
                break;
            case Key::scan::down:
                keyboard.set_repeat_rate(cur_speed, (cur_delay == 0) ? 0 : --cur_delay);
                break;
            }
            continue;
        }

        kout_mutex.lock();

        int x, y;
        kout.getpos(x, y);
        kout.setpos(40 + pos % 36, 2 + pos / 36);
        kout << k.ascii() << flush;
        kout.setpos(x, y);

        kout_mutex.unlock();
    }
}

void KeyboardApplication::setID(int i) {
    id = i;
}
