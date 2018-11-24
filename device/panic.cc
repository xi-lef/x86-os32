// vim: set et ts=4 sw=4:

#include "device/panic.h"
#include "debug/output.h"
#include "machine/cpu.h"

Panic panic;

void Panic::trigger() {
    DBG << "panic!!1" << endl;
    CPU::die();
}
