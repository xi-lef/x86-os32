
#include "user/app2/kappl.h"
#include "syscall/guarded_semaphore.h"
#include "device/cgastr.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"
#include "utils/random.h"
#include "user/shell/shell.h"

void KeyboardApplication::action() {
    //Random r(1337);
    int cur_speed = 0;
    int cur_delay = 0;

    // this writes in a 36 columns and 11 rows big area of kout
    //for (int pos = 0; ; ++pos %= 36 * 11) {
    for (;;) {
        //char *test = new char[r.number() % (1024 * 900)];
        Key k = keyboard.getkey();
        //delete[] test;

        if (k.ctrl()) {
            if (k.ascii() == 't') {
                Shell shell(kout);
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

        //int x_save, y_save;
        //kout.getpos(x_save, y_save);
        //kout.setpos(x, y);
        if (k.ascii() == '\b') {
            kout << backspace;
        } else {
            kout << k.ascii() << flush;
        }
        //kout.getpos(x, y);
        //kout.setpos(x_save, y_save);

        kout_mutex.unlock();
    }
}

void KeyboardApplication::setID(int i) {
    id = i;
}
