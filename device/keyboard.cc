// vim: set et ts=4 sw=4:

#include "device/keyboard.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"

Keyboard keyboard;

void Keyboard::plugin() {
    Plugbox::Vector kb_vector = Plugbox::Vector::keyboard;
    unsigned char   kb_slot   = system.getIOAPICSlot(APICSystem::Device::keyboard);

    plugbox.assign(kb_vector, &keyboard);
    ioapic.config(kb_slot, kb_vector);
    ioapic.allow(kb_slot);
    drainKeyboardBuffer();
}

void Keyboard::trigger() {
    Key k = key_hit();
    if (!k.valid()) {
        //DBG << "invalid key" << flush;
        return;
    }

    if (k.ctrl() && k.alt() && (k.scancode() == Key::scan::del)) {
        DBG << endl << "REBOOTING..." << endl;
        sleep(2);
        reboot();
    }

    dout_status << k.ascii() << flush;
}
