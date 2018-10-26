// vim: set et ts=4 sw=4:

#pragma once

#include "types.h"

enum {
	KERNEL_CODE_SEGMENT = 1,
	KERNEL_DATA_SEGMENT = 2,
	KERNEL_16DATA_SEGMENT = 3,
	KERNEL_16CODE_SEGMENT = 4,
};

/** \brief Diese Klasse beschreibt den Inhalt eines einzelnen GDT Eintrages 
 *
 * Im 32-Bit Modus hat ein GDT Eintrag 8 Byte. Die 64 Bit werden von
 * dieser Struktur in einzeln ansprechbare Bitfelder aufgeteilt. Des
 * weiteren enthält die Klasse constexpr Konstruktoren, die es
 * erlauben zur Übersetzungszeit bekannte Instanzen zu erzeugen.
*/
class GDTDescriptor {
public:
	union {
		struct
		{
			unsigned int limit_low:16;
			unsigned int base_low : 24;
			// Das Attribut Byte wurde in einzelne Bitfelder aufgeteilt
			unsigned int accessed :1;
			unsigned int read_write :1; //!< Code: Lesbar? Daten: Schreibar?
			unsigned int conforming_expand_down :1; //!< Code: Conforming; Daten: Richtungsbit
			unsigned int code :1; //!< Code: 1; Daten: 0
			unsigned int always_1 :1; //!< should be 1 for everything but TSS and LDT
			unsigned int DPL :2; //!< Das Schutzlevel für diesen Deskriptor
			unsigned int present :1; //!< Ist dieser GDT Eintrag valide?
			unsigned int limit_high :4;
			unsigned int available :1;
			unsigned int always_0 :1; // should always be 0
			unsigned int big :1; // Code: 32bit Befehle; Daten: 4 Byte pro Stackeintrag
			unsigned int granularity :1; // 1: Limit * 4096; 0: Limit ist in Bytes gegeben
			unsigned int base_high :8;
		} __attribute__((packed));

		uint64_t value; //!< Der zusammengebaute Bitstring
	} __attribute__((packed));

	/** \brief NULL-Deskriptor */
	constexpr GDTDescriptor() : value(0) {};

	/** \brief Konstruktor für einen konkret gegebenen Eintrag */
	constexpr GDTDescriptor(uint64_t val) : value(val) {};

	/** \brief Konstruktor für einen 16/32 Bit Code/Memory Eintrag */
	constexpr GDTDescriptor(uint32_t base, uint32_t limit,
							bool code, bool user, bool _32_bit = true)
		: limit_low((limit > 0xffff)
					? ((limit >> 12) & 0x0000FFFF)
					: (limit & 0x0000FFFF)),
		base_low(base & 0x00FFFFFF),
		accessed(0),
		read_write(1),
		conforming_expand_down(0),
		code(code ? 1 : 0),
		always_1(1),
		DPL(user ? 3 : 0),
		present(1),
		limit_high((limit > 0xffff)
				   ? ((limit >> 28) & 0xF)
				   : ((limit >> 16) & 0xF)),
		available(0),
		always_0(0),
		big(_32_bit), // 32 Bit Modus?
		granularity(limit > 0xffff), // 4096 Byte Granularität
		base_high((base & 0xFF000000) >> 24)
	{ /* Leerer Konstruktor */};

} __attribute__((packed));

/** \brief Diese Struktur beschreibt einen GDT Zeiger. Er enthält die
	Länge der GDT in Bytes (minus 1 Byte) und den Pointer auf die
	GDT. Dieser Pointer wird zusammen mit `lgdt` verwendet. */
struct GlobalDescriptorTable {
	uint16_t limit; //!< GDT Größe in Bytes (minus 1 Byte)
	const GDTDescriptor* base; //!< GDT Basisaddresse
} __attribute__((packed));

extern "C"  GDTDescriptor gdt_descriptors[];

