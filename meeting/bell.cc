// vim: set et ts=4 sw=4:

#include "meeting/bell.h"
#include "meeting/bellringer.h"
#include "thread/scheduler.h"
#include "debug/output.h"

void Bell::ring() {
    Thread *t;
    // this also considers the possibility that no thread was in the queue,
    // which might happen if the only thread in the queue was killed.
    while ((t = dequeue())) {
        scheduler.wakeup(t);
    }
}

void Bell::sleep(unsigned int ms) {
    if (ms == 0) {
        return;
    }

    Bell bell;
    bellringer.job(&bell, ms);
    scheduler.block(&bell);
}
