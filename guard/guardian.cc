// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Low-Level Interrupt-Behandlung
 */
#include "types.h"
#include "guardian.h"
#include "debug/output.h"
#include "machine/lapic.h"
#include "machine/plugbox.h"

extern "C" void guardian(uint32_t vector, irq_context *context) {
    (void) context;
    DBG << "interrupt!!1" << flush;
    (plugbox.report(vector))->trigger();
    lapic.ackIRQ();
}
