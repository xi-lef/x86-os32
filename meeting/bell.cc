// vim: set et ts=4 sw=4:

#include "meeting/bell.h"
#include "meeting/bellringer.h"
#include "thread/scheduler.h"
#include "debug/output.h"

void Bell::ring() {
    scheduler.wakeup(Queue::first());
}

void Bell::sleep(unsigned int ms) {
    if (ms == 0) {
        return;
    }

    Bell bell(ms);
    bellringer.job(&bell, ms);
    scheduler.block(&bell);
}
