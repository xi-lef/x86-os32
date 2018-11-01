#include "assert.h"

#include "debug/output.h"
#include "device/cgastr.h"
#include "machine/cpu.h"

[[noreturn]] void assertion_failed(const char * exp, const char * func, const char * file, int line){
	DBG << "Assertion '" << exp << "' failed (" << func << " @ " << file
	    << ":" << line << ") - CPU stopped." << endl;
	CPU::die();
	while(1);
}

