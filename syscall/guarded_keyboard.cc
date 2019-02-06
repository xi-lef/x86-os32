// vim: set et ts=4 sw=4:

#include "syscall/guarded_keyboard.h"
#include "guard/secure.h"

Guarded_Keyboard keyboard;

Key Guarded_Keyboard::getkey() {
    Secure s;
    return Keyboard::getkey();
}
