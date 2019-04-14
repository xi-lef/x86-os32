#include "syscall/guarded_scheduler.h"
#include "guard/secure.h"
#include "user/status/status.h"

void Guarded_Scheduler::exit() {
    Secure s;
    status.thread_dec();
    scheduler.active()->Thread::~Thread();
    scheduler.exit();
}

void Guarded_Scheduler::kill(Thread *that) {
    Secure s;
    status.thread_dec();
    scheduler.kill(that);
}

void Guarded_Scheduler::ready(Thread *that) {
    Secure s;
    status.thread_inc();
    scheduler.ready(that);
}

void Guarded_Scheduler::resume() {
    Secure s;
    scheduler.resume();
}

void Guarded_Scheduler::wakeup(Thread *customer) {
    Secure s;
    scheduler.wakeup(customer);
}
