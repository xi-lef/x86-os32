
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

#if 0
        //DBG << "pre" << endl;
        String str("abcdabcdabcd");
        //DBG << "post" << endl;
        kout << str << " (len=" << str.length() << ")" << endl;
        continue;
#endif

#if 0
        String str("test123");
        for (char c : str) {
        //for (auto c = str.begin(); c != str.end(); c++) {
            DBG << c << ' ' << flush;
        }
#endif

#if 0
        char *bla = (char *) malloc(10);
        bla[9] = '\0';
        kout << "0 " << (void *) bla << '=' << bla << endl;
        //int tmp = String("abcd").copy(bla, 10);
        //bla[tmp] = '\0';
        bla[0] = 'a'; bla[1] = 'b'; bla[2] = 'c'; bla[3] = 'd'; bla[4] = '\0';
        kout << "1 " << (void *) bla << '=' << bla << endl;
        bla = (char *) realloc(bla, 20);
        kout << "2 " << (void *) bla << '=' << bla << endl;
        free(bla);
#endif

        if (k.ascii() == 't') {
            Shell shell(kout);
            kout_mutex.lock();
            shell.start();
            kout_mutex.unlock();
            continue;
        }

        if (k.ctrl()) {
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
