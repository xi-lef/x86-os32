// vim: set et ts=4 sw=4:

#include "user/app2/kappl.h"
#include "syscall/guarded_semaphore.h"
#include "device/cgastr.h"
#include "debug/output.h"
#include "syscall/guarded_keyboard.h"

void KeyboardApplication::action() {
    kout_sem.p();

    int x, y;
    kout.getpos(x, y);
    kout.setpos(40, 1);
    kout << "KB_App: " << flush;
    kout.setpos(x, y);

    kout_sem.v();

    // this writes in a 36 columns and 11 rows big area of kout
    for (int pos = 0; ; ++pos %= 36 * 11) {
        Key k = keyboard.getkey();

        kout_sem.p();

        int x, y;
        kout.getpos(x, y);
        kout.setpos(40 + pos % 36, 2 + pos / 36);
        kout << k.ascii() << flush;
        kout.setpos(x, y);

        kout_sem.v();
    }
}

void KeyboardApplication::setID(int i) {
    id = i;
}
