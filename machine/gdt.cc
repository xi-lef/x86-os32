// vim: set et ts=4 sw=4:

#include "machine/gdt.h"
#include "debug/output.h"

// Die statische Global Descriptor Tabelle
GDTDescriptor gdt_descriptors[] __attribute__ ((aligned (8))) = {
	// NULL-Deskriptor
	{},

	// Globales Codesegment von 0-4GB
	{/* base  = */ 0x0,
	 /* limit = */ 0xFFFFFFFF,
	 /* code  = */ true,
	 /* user  = */ false},

	// Globales Datensegment von 0-4GB
	{/* base  = */ 0x0,
	 /* limit = */ 0xFFFFFFFF,
	 /* code  = */ false,
	 /* user  = */ false},

	// 16-Bit Datensegent von 0x90000-0x9FFFF zum "Zurückschalten" in den Real Mode
	{/* base  = */ 0x90000,
	 /* limit = */ 0xFFFF,
	 /* code  = */ false,
	 /* user  = */ false,
	 /* 32 Bit= */ false},

	// 16-Bit Codesegment von 0x90000-0x9FFFF zum "Zurückschalten" in den Real Mode
	{/* base  = */ 0x90000,
	 /* limit = */ 0xFFFF,
	 /* code  = */ true,
	 /* user  = */ false,
	 /* 32 Bit= */ false},
};

//!< Der statische GDT Zeiger. Dieser wird von lgdt in boot/startup.asm verwendet
extern "C"
constexpr GlobalDescriptorTable gdt_desc_global __attribute__ ((aligned(8))) = {
	sizeof(gdt_descriptors) - 1,
	&gdt_descriptors[0]
};

