#include "syscall/guarded_scheduler.h"
#include "guard/secure.h"

void Guarded_Scheduler::exit() {
    Secure s;
    scheduler.exit();
}

void Guarded_Scheduler::kill(Thread *that) {
    Secure s;
    scheduler.kill(that);
}

void Guarded_Scheduler::ready(Thread *that) {
    Secure s;
    scheduler.ready(that);
}

void Guarded_Scheduler::resume() {
    Secure s;
    scheduler.resume();
}
