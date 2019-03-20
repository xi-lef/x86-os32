#pragma once

#include "user/mutex/mutex.h"
#include "guard/secure.h"

class Guarded_Mutex : public Mutex {
public:
    void lock();
    bool unlock();
};
