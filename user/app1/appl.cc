// vim: set et ts=4 sw=4:

#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"

void Application::action() {
    for (int16_t i = 0; ; i++) {
        //DBG << "App " << id << ": action " << flush;
        { Secure s;
            int x, y;
            kout.getpos(x, y);
            kout.setpos(4, id + 2);
            kout << "App " << id << ": " << i << flush;
            kout.setpos(x, y);
        }
    }
}

void Application::setID(int i) {
    id = i;
}
