#include "meeting/semaphore.h"
#include "thread/scheduler.h"
#include "machine/cpu.h"
#include "debug/output.h"

void Semaphore::p() {
    if (counter > 0) {
        counter--;
    } else {
        scheduler.block(this);
    }
}

void Semaphore::v() {
    Thread *t;
    if ((t = dequeue()) != nullptr) {
        scheduler.wakeup(t);
    } else {
        counter++;
    }
}
