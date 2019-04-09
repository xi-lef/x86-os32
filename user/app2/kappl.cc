
#include "user/app2/kappl.h"
#include "syscall/guarded_semaphore.h"
#include "device/cgastr.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"
#include "utils/random.h"
#include "user/shell/shell.h"

void KeyboardApplication::action() {
    int cur_speed = 0;
    int cur_delay = 0;

    for (;;) {
        Key k = keyboard.getkey();

        /*String str("hallo du");
        kout << COLOR_YELLOW << str << COLOR_RESET << ' ';
        String sub = str.substr(2, 3);
        kout << COLOR_YELLOW << sub << COLOR_RESET << endl;
        continue;*/

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
