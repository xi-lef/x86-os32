// vim: set et ts=4 sw=4:

/* Systemerkennung mit ACPI
 * Wer hier tiefer einsteigen will, der darf das gerne tun.
 * Hinterher aber bitte nicht über Knoten im Hirn oä. beschweren.
 */

#pragma once

#include "machine/apicsystem.h"
#include "compiler.h"

#include "debug/assert.h"
#include "debug/output.h"

namespace ACPI_Subsystem
{
typedef unsigned int UInt32;
#define CHARS_TO_UINT32(a,b,c,d) (((a))|((b)<<8)|((c)<<16)|(d)<<24)

bool sums_to_zero( void *start, int len );

//spec 5.2.11.4
class System_Description_Table //Generic APIC SDT Header
{
public:
	enum {
		RSDT = CHARS_TO_UINT32('R','S','D','T'),
		XSDT = CHARS_TO_UINT32('X','S','D','T'),
		MADT = CHARS_TO_UINT32('A','P','I','C'),
	};

	char signature[4];
protected:
	UInt32 length;
private:
	char revision;
	char checksum;
	char oem_id[6];
	char oem_table_id[8];
	UInt32 oem_revision;
	UInt32 creator_id;
	UInt32 creator_revision;

	bool check_type( UInt32 SDTtype );

public:
	UInt32 getLength() {
		return length;
	}
	bool check_sum() {
		return sums_to_zero( this, getLength() );
	}
	char *getSignature() {
		return signature;
	}
	UInt32 getType() {
		return CHARS_TO_UINT32( signature[0], signature[1],
		                        signature[2], signature[3] );
	}
};
assert_size(System_Description_Table, 36);

//spec 5.2.11.4
class APIC_Structure
{
public:
	//there are lots of other values (see spec table 5-19)
	enum { LOCAL_APIC=0, IO_APIC=1, SOURCE_OVERRIDE=2 };
private:
	unsigned char type; //our subclass tag
	unsigned char length;
public:
	unsigned char getType() {
		return type;
	}
	unsigned char getLength() {
		return length;
	}
	APIC_Structure *getNextAPICStructurePtr() {
		return (APIC_Structure *)
		       (( (char *) this ) + getLength() );
	}
};
assert_size(APIC_Structure, 2);

//spec 5.2.11.5
class Local_APIC_Structure : public APIC_Structure
{
	// + type, length!
	char apic_processor_id;
	char apic_id;
	UInt32 flags;

public:
	char getAPIC_ID() {
		return apic_id;
	}
	char getProcessorID() {
		return apic_processor_id;
	}
	//spec Table 5-21 and 5.2.1.1
	bool isEnabled() {
		return flags & 1;
	}
};
assert_size(Local_APIC_Structure, sizeof( APIC_Structure ) + 6);

//spec 5.2.11.6
class IO_APIC_Structure : public APIC_Structure
{
	// + type, length!
	char   apic_id;
	char   reserved__ UNUSED_STRUCT_FIELD;
	void*  apic_address;
	UInt32 global_system_interrupt_base UNUSED_STRUCT_FIELD;

public:
	char getAPIC_ID() {
		return apic_id;
	}
	void *getAPIC_Adress() {
		return apic_address;
	}
};
assert_size(IO_APIC_Structure, sizeof( APIC_Structure ) + 10);

//spec 5.2.12.5
class Int_Source_Override_Structure : public APIC_Structure
{
	// + type, length!
	unsigned char bus;
	unsigned char source;
	unsigned int global_system_interrupt;
	unsigned short flags UNUSED_STRUCT_FIELD;
public:
	unsigned char getBus() {
		return bus;
	}
	unsigned char getSource() {
		return source;
	}
	unsigned long getGlobalSystemInterrupt() {
		return global_system_interrupt;
	}
} __attribute__((packed));
assert_size(Int_Source_Override_Structure, sizeof( APIC_Structure ) + 8);

//spec 5.2.11.4
class MADT : public System_Description_Table //Multiple APIC Description Table
{
	void *local_apic_address UNUSED_STRUCT_FIELD;
	UInt32 flags;

public:
	APIC_Structure *getFirstAPICStructurePtr() {
		return (APIC_Structure *) (this+1);
	}
	APIC_Structure *getRearPtr() {
		return (APIC_Structure *) ( (char *) this  + getLength() );
	}
	//spec table 5-18 and 5.2.1.1
	bool has_PCAT_COMPAT() {
		return flags & 1;
	}
};
assert_size(MADT, sizeof( System_Description_Table ) + 8);

//spec 5.2.7
class RSDT : public System_Description_Table //Root System Description Table
{
public:
	/* return the number of contained pointers to other system description
	 * tables (i.e. MADT) */
	UInt32 getNumSDTPtrs() {
		return ( length - sizeof( System_Description_Table ) ) / 4;
	}
	/* return the nth SDT pointer (starting with 0, it's C ;-)*/
	System_Description_Table *getSDTPtr(int num) {
		return ( (System_Description_Table **) (this+1) ) [num];
	}
};

//spec 5.2.8
class XSDT : public System_Description_Table //Xtended System Description Table
{
public:
	/* return the number of contained 64-bit pointers to other system
	   description tables (i.e. MADT) */
	UInt32 getNumSDTPtrs() {
		return ( length - sizeof( System_Description_Table ) ) / 8;
	}
	/* return the nth SDT pointer (starting with 0, it's C ;-)*/
	System_Description_Table *getSDTPtr(int num) {
		System_Description_Table **ptr64 = (System_Description_Table **) (this+1) + (num << 1);

		if ( ptr64[1] == 0 )
			return ptr64[0];

		DBG_VERBOSE << "ACPI: found a real 64-Bit pointer in the XSDT -> "
		            << "getSDTPtr() will return NULL" << endl;

		return 0;
	}
};

//spec 5.2.5.3
class RSDP
{
	//since ACPI 1.0
	char signature[8] UNUSED_STRUCT_FIELD;
	char checksum UNUSED_STRUCT_FIELD;
	char oemid[6] UNUSED_STRUCT_FIELD;
	char revision;
	RSDT *rsdt_address;
	//since... later
	UInt32 length;
	/* The following pointer should actually be a 64-bit pointer.
	   Since the OOStuBS build environment only generates 32bit code
	   (anything else wouldn't work... for now) we have to use
	   32 pointers here and hope for the high part being 0*/
	XSDT *xsdt_address_lo;
	UInt32 xsdt_address_hi;
	static RSDP *find( void *start, int len );

	bool check_sum() {
		if ( is_ACPI_1_0() )
			return sums_to_zero ( this, 20 );
		else
			return sums_to_zero ( this, length );
	}
public:
	static RSDP *find();
	bool is_ACPI_1_0() {
		return revision == 0;
	}
	RSDT *getRSDTPtr() {
		return rsdt_address;
	}
	XSDT *getXSDTPtr() {
		if ( is_ACPI_1_0() ) {

			DBG_VERBOSE << "ACPI: tried to get the XSDT adress although we"
			            << "have ACPI 1.0 -> returning NULL" << endl;

			return 0;
		}
		if ( xsdt_address_hi != 0 ) {

			DBG_VERBOSE << "ACPI: pointer to XSDT is > 4GB (real 64bit)"
			            << "-> returning NULL" << endl;

			return 0;
		}
		return xsdt_address_lo;
	}
};
assert_size(RSDP, 32);

} //ACPI_Subsystem

