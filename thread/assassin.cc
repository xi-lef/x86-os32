// vim: set et ts=4 sw=4:

#include "thread/assassin.h"
#include "machine/plugbox.h"
#include "syscall/guarded_scheduler.h"
#include "debug/output.h"

Assassin assassin;

void Assassin::hire() {
    plugbox.assign(Plugbox::Vector::assassin, &assassin);
}

bool Assassin::prologue() {
    return true;
}

void Assassin::epilogue() {
    Thread *active = scheduler.active();
    if (active->dying()) {
        scheduler.exit();
    }
}
