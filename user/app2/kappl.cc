// vim: set et ts=4 sw=4:

#include "user/app2/kappl.h"
#include "device/keyboard.h"
#include "syscall/guarded_semaphore.h"
#include "device/cgastr.h"
#include "debug/output.h"

extern Guarded_Semaphore kout_sem;

void KeyboardApplication::action() {
    for (;;) {
        Key k = keyboard.getkey();

        //DBG << "pre kout p " << flush;
        kout_sem.p();
        //DBG << "post kout p " << flush;

        int x, y;
        kout.getpos(x, y);
        kout.setpos(20, id - 10);
        kout << "KB_App " << id << ": " << k.ascii() << flush;
        kout.setpos(x, y);

        kout_sem.v();
    }
}

void KeyboardApplication::setID(int i) {
    id = i;
}
