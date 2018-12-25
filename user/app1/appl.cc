// vim: set et ts=4 sw=4:

#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"

extern Application *apps[16];

void Application::action() {
    for (uint32_t i = 0; ; i++) {
        DBG << "Thread " << id << ": action " << flush;
        //Secure s; // TODO change for resume
        int x, y;
        kout.getpos(x, y);
        kout.setpos(4, id + 2);
        kout << "Thread " << id << ": " << i << flush;
        kout.setpos(x, y);
        resume(apps[id == 0 ? 1 : 0]);
    }
}

void Application::setID(int i) {
    id = i;
}
