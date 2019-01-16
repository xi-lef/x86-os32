// vim: set et ts=4 sw=4:

#include "stub.h"
#include "debug/output.h"

/*!\brief Array mit Addressen der jeweiligen spezifischen Debug-Unterbrechungsbehandlungsroutinen
 * aus der handler.asm ( dbg_irq_entry_%INDEX )
 */
extern void const * const dbg_irq_entries[];

bool GDB_Stub::install_handler(int vector){
    if (vector < 0 || vector > 255) {
        DBG << "GDB: invalid vector" << endl;
        return false;
    }

    extern uint64_t idt[]; // from boot/startup.asm

    uint64_t idt_entry = 0;
    idt_entry |=  (uint64_t) dbg_irq_entries[vector] & 0xffff;            // low 2 bytes of address
    idt_entry |= (uint64_t) 0x8e000008 << 16;                             // idt flags/options
    idt_entry |= ((uint64_t) dbg_irq_entries[vector] & 0xffff0000) << 32; // high 2 bytes of address

    idt[vector] = idt_entry;

	return true;
}

