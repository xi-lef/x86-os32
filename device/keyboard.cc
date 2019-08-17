// vim: set et ts=4 sw=4:

#include "device/keyboard.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/rtc.h"
#include "syscall/guarded_keyboard.h"

void Keyboard::plugin() {
    Plugbox::Vector kb_vector = Plugbox::Vector::keyboard;
    unsigned char   kb_slot   = system.getIOAPICSlot(APICSystem::Device::keyboard);

    plugbox.assign(kb_vector, this);
    ioapic.config(kb_slot, kb_vector, TRIGGER_MODE_LEVEL);
    ioapic.allow(kb_slot);
    drainKeyboardBuffer();
}

extern CGA_Stream kout;

bool Keyboard::prologue() {
    bool ret = false; // in case of spurious interrupts, dont request epilogue

    Key k;
    while (k = key_hit(), k.valid()) {
        if (k.ctrl() && k.alt() && (k.scancode() == Key::scan::del)) {
            reboot();
        }

        if (prebuf.produce(k)) {
            ret = true;
        }
    }
 
    return ret;
}

void Keyboard::epilogue() {
    Key k;
    while (prebuf.consume(k)) {
        buf.produce(k);
        sem.v();
    }
}

Key Keyboard::getkey() {
    sem.p();
    Key k;
    buf.consume(k);
    return k;
}

size_t Keyboard::read(String *s, size_t count, CGA_Stream& out) {
    size_t i;
    for (i = 0; i < count; i++) {
        Key k = getkey();

        // ctrl + d makes read terminate
        if (k.ctrl() && (k.ascii() == 'd' || k.ascii() == 'D')) {
            break;
        }

        // catch backspace
        if (k.ascii() == '\b') {
            out.backspace(s);
            continue;
        }

        s->append(k.ascii());
        out << k.ascii() << flush;

        if (k.ascii() == '\n') {
            break;
        }
    }

    return i;
}

Keyboard& Keyboard::operator >>(char& c) {
    c = getkey().ascii();
    return *this;
}

Keyboard& Keyboard::operator >>(String& s) {
    read(&s, 4096);
    return *this;
}

Keyboard& Keyboard::operator >>(long& i) {
    String s;
    read(&s, 4096);
    i = strtol(s);
    return *this;
}
