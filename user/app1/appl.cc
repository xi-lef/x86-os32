// vim: set et ts=4 sw=4:

#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"
#include "syscall/guarded_semaphore.h"
#include "syscall/guarded_bell.h"
#include "utils/math.h"

void Application::action() {
    unsigned int ms = Math::pow(2, id) * 8;
    int limit = 256;

    kout_mutex.lock();

    int x, y;
    kout.getpos(x, y);
    kout.setpos(4, id + 2);
    kout << "App " << id << " (ms: " << ms << "): " << flush;
    kout.setpos(x, y);

    kout_mutex.unlock();

    for (int16_t i = 0; i < limit; i++) {
        //DBG << "App " << id << ": action " << flush;
        kout_mutex.lock();

        int x, y;
        kout.getpos(x, y);
        kout.setpos(23, id + 2);
        kout << i << flush;
        kout.setpos(x, y);
        //if (id == 0 && i == 1337) {
        //    Guarded_Scheduler::exit();
        //    Guarded_Scheduler::kill(this);
        //}

        kout_mutex.unlock();
        Guarded_Bell::sleep(ms);
    }

    Guarded_Scheduler::exit();
}

void Application::setID(int i) {
    id = i;
}
