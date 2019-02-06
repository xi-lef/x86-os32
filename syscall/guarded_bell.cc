// vim: set et ts=4 sw=4:

#include "syscall/guarded_bell.h"
#include "guard/secure.h"

void Guarded_Bell::sleep(unsigned int ms) {
    Secure s;
    Bell::sleep(ms);
}
