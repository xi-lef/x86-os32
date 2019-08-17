// vim: set et ts=4 sw=4:

#include "syscall/guarded_keyboard.h"
#include "guard/secure.h"

Guarded_Keyboard keyboard;

Key Guarded_Keyboard::getkey() {
    Secure s;
    return Keyboard::getkey();
}

Guarded_Keyboard& Guarded_Keyboard::operator >>(char &c) {
    Secure s;
    Keyboard::operator>>(c);
    return *this;
}

Guarded_Keyboard& Guarded_Keyboard::operator >>(String &str) {
    Secure s;
    Keyboard::operator>>(str);
    return *this;
}

Guarded_Keyboard& Guarded_Keyboard::operator >>(long &i) {
    Secure s;
    Keyboard::operator>>(i);
    return *this;
}

void Guarded_Keyboard::set_repeat_rate(int speed, int delay) {
    Secure s;
    Keyboard::set_repeat_rate(speed, delay);
}

void Guarded_Keyboard::set_led(led_t led, bool on) {
    Secure s;
    Keyboard::set_led(led, on);
}
