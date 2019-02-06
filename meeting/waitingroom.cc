// vim: set et ts=4 sw=4:

#include "meeting/waitingroom.h"
#include "syscall/guarded_scheduler.h"

Waitingroom::~Waitingroom() {
    Thread *t;
    while ((t = dequeue()) != nullptr) {
        Guarded_Scheduler::wakeup(t);
    }
}

void Waitingroom::remove(Thread *customer) {
    Queue::remove(customer);
}
