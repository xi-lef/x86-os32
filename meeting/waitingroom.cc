// vim: set et ts=4 sw=4:

#include "meeting/waitingroom.h"
#include "thread/scheduler.h"

Waitingroom::~Waitingroom() {
    Thread *t;
    while ((t = dequeue()) != nullptr) {
        scheduler.wakeup(t);
    }
}

void Waitingroom::remove(Thread *customer) {
    Queue::remove(customer);
}
