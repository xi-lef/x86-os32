#include "user/mutex/mutex.h"
#include "thread/scheduler.h"

void Mutex::lock() {
    Semaphore::p();
    owner = scheduler.active();
    owner->mutex_hold(this);
}

bool Mutex::unlock() {
    bool ret = owner->mutex_release(this);
    owner = nullptr;
    Semaphore::v();
    return ret;
}
