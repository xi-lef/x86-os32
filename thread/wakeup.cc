#include "thread/wakeup.h"
#include "machine/plugbox.h"

WakeUp wakeup;

void WakeUp::activate() {
    plugbox.assign(Plugbox::Vector::wakeup, this);
}

bool WakeUp::prologue() {
    return false;
}
