// vim: set et ts=4 sw=4:

#include "syscall/guarded_keyboard.h"
#include "guard/secure.h"

Guarded_Keyboard keyboard;

Key Guarded_Keyboard::getkey() {
    Secure s;
    return Keyboard::getkey();
}

void Guarded_Keyboard::set_repeat_rate(int speed, int delay) {
    Secure s;
    Keyboard::set_repeat_rate(speed, delay);
}

void Guarded_Keyboard::set_led(led_t led, bool on) {
    Secure s;
    Keyboard::set_led(led, on);
}
