// vim: set et ts=4 sw=4:

#include "device/keyboard.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/rtc.h"

Keyboard keyboard;

void Keyboard::plugin() {
    Plugbox::Vector kb_vector = Plugbox::Vector::keyboard;
    unsigned char   kb_slot   = system.getIOAPICSlot(APICSystem::Device::keyboard);

    plugbox.assign(kb_vector, &keyboard);
    ioapic.config(kb_slot, kb_vector, TRIGGER_MODE_LEVEL);
    ioapic.allow(kb_slot);
    drainKeyboardBuffer();
}

extern CGA_Stream kout;

bool Keyboard::prologue() {
    bool ret = false; // in case of spurious interrupts, dont request epilogue

    for (Key k = key_hit(); k.valid(); k = key_hit()) {
        ret = true;
        if (k.ctrl() && k.alt() && (k.scancode() == Key::scan::del)) {
            reboot();
        }

        if (!buf.produce(k)) {
            DBG << "KB: buffer full :( " << flush;
            while (key_hit().valid()) ;
            break;
        }
        count++;
    }
 
    return ret;
}

void Keyboard::epilogue() {
    /*
    Key k;
    while (buf.consume(k)) {
        kout << k.ascii();
    }
    kout << flush;
    */
    //DBG << "count: " << count << " " << flush;
    while (count > 0) {
        count--;
        sem.v();
    }
}

Key Keyboard::getkey() {
    //DBG << "pre getkey p " << flush;
    guard.enter(); // TODO
    sem.p();
    guard.leave();
    //DBG << "post getkey p " << flush;
    Key k;
    buf.consume(k);
    return k;
}
