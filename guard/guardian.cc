// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Low-Level Interrupt-Behandlung
 */
#include "types.h"
#include "guardian.h"

extern "C" void guardian(uint32_t vector, irq_context *context)
{
	(void) vector;
	(void) context;

}
