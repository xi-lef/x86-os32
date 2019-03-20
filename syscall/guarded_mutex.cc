#include "syscall/guarded_mutex.h"

void Guarded_Mutex::lock() {
    Secure s;
    Mutex::lock();
}

bool Guarded_Mutex::unlock() {
    Secure s;
    return Mutex::unlock();
}
