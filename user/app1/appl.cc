// vim: set et ts=4 sw=4:

#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"
#include "syscall/guarded_semaphore.h"
#include "syscall/guarded_bell.h"
#include "utils/math.h"

Guarded_Semaphore kout_sem(1);

void Application::action() {
    for (int16_t i = 0; ; i++) {
        //DBG << "App " << id << ": action " << flush;
        kout_sem.p();

        int x, y;
        kout.getpos(x, y);
        kout.setpos(4, id + 2);
        kout << "App " << id << ": " << i << flush;
        kout.setpos(x, y);

        kout_sem.v();
        Guarded_Bell::sleep(Math::pow(2, id) * 50);
    }
}

void Application::setID(int i) {
    id = i;
}
