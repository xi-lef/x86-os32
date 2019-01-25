// vim: set et ts=4 sw=4:

#include "stub.h"
#include "debug/output.h"
#include "debug/gdb/idt.h"

/*!\brief Array mit Addressen der jeweiligen spezifischen Debug-Unterbrechungsbehandlungsroutinen
 * aus der handler.asm ( dbg_irq_entry_%INDEX )
 */
extern void const * const dbg_irq_entries[];
extern const int dbg_irq_entries_length;

bool GDB_Stub::install_handler(int vector){
    if (vector < 0 || vector > dbg_irq_entries_length - 1) {
        DBG << "GDB: invalid vector" << endl;
        return false;
    }

    idt_entry idt_e = idt[vector];
    idt_e.address_low  = (uint32_t) dbg_irq_entries[vector] & 0xffff;
    idt_e.address_high = ((uint32_t) dbg_irq_entries[vector] & 0xffff0000) >> 16;
    idt[vector] = idt_e;

    //DBG << "GDB: installed " << vector << " " << flush;
	return true;
}

