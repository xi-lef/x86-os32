#include "meeting/semaphore.h"
#include "thread/scheduler.h"
#include "machine/cpu.h"
#include "debug/output.h"

void Semaphore::p() {
    if (counter > 0) {
        counter--;
    } else {
        //DBG << "sem p block " << flush;
        scheduler.block(this);
    }
}

void Semaphore::v() {
    //DBG << "Sem: v " << flush;
    Thread *t;
    if ((t = dequeue()) != nullptr) {
        //DBG << "someone was waiting " << flush;
        scheduler.wakeup(t);
    } else {
        //DBG << "counter++ " << flush;
        counter++;
    }
}
