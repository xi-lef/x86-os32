// vim: set et ts=4 sw=4:

#include "user/status/sappl.h"
#include "user/status/status.h"
#include "device/cgastr.h"
#include "syscall/guarded_bell.h"

void StatusApplication::action() {
    for (;;) {
        dout_status.reset();
        dout_status << "idle CPUs: ";
        for (int i = 0; i < CPU_MAX; i++) {
            if (status.cpu_idle[i]) {
                dout_status << i;
            }
        }
        dout_status.flush();

        dout_status.setpos(19, 24);
        dout_status << "#threads: " << status.thread_counter;

        dout_status.flush();
        Guarded_Bell::sleep(33);
    }
}

void StatusApplication::setID(int i) {
    id = i;
}
