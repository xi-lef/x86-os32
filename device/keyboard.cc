// vim: set et ts=4 sw=4:

#include "device/keyboard.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"
#include "machine/cpu.h"
#include "machine/ticketlock.h"
#include "object/bbuffer.h"

Keyboard keyboard;
//static Key buf;
static BBuffer<Key, 16> buf[4];

void Keyboard::plugin() {
    Plugbox::Vector kb_vector = Plugbox::Vector::keyboard;
    unsigned char   kb_slot   = system.getIOAPICSlot(APICSystem::Device::keyboard);

    plugbox.assign(kb_vector, &keyboard);
    ioapic.config(kb_slot, kb_vector);
    ioapic.allow(kb_slot);
    drainKeyboardBuffer();
}

extern CGA_Stream kout;

// TODO fucked order of keys?
bool Keyboard::prologue() {
    //DBG << "KB: prologue " << flush;
    int id = system.getCPUID();
    bool ret = false; // in case of spurious interrupts, dont request epilogue

    for (Key k = key_hit(); k.valid(); k = key_hit()) {
        ret = true;
        if (k.ctrl() && k.alt() && (k.scancode() == Key::scan::del)) {
            DBG << endl << "REBOOTING..." << endl;
            //sleep(2);
            reboot();
        }

        if (!buf[id].produce(k)) {
            DBG << "bbuffer full :( " << flush;
            while (key_hit().valid()) ;
            break;
        }
    }
 
    return ret;
}

void Keyboard::epilogue() {
    //DBG << "KB: epilogue " << flush;
    int id = system.getCPUID();
    Key k;
    CPU::disable_int();
    while (buf[id].consume(k)) {
        CPU::enable_int();
        kout << k.ascii();
        CPU::disable_int();
    }
    CPU::enable_int();
    kout << flush;
}
