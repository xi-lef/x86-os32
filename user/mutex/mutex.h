#pragma once

#include "meeting/semaphore.h"
#include "object/queuelink.h"
#include "thread/thread.h"

class Mutex : public Semaphore {
private:
    Thread *owner;

public:
    QueueLink<Mutex> queue_link;

    Mutex() : Semaphore(1), owner(nullptr) {}

    void lock();
    bool unlock();
};
