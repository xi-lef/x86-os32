// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält ein Makro um eine Fehlermeldung auszugeben und die CPU
 *  anzuhalten
 */

#pragma once

/*!
 *  \brief Gebe eine Fehlermeldung aus und stoppe die CPU
 *  \ingroup debug
 *  \param MSG anzuzeigende Fehlermeldung
 */
#define kernelpanic(MSG) \
	do { \
			DBG << "PANIC: '" << (MSG) << "' in " << __func__ \
			    << " @ " << __FILE__ << ":" << __LINE__ << ") - CPU stopped." \
			    << endl; \
		CPU::die(); \
	} while(0)

// Includes am Ende, damit das Makro auch darin verwendet werden könnte
#include "debug/output.h"
#include "machine/cpu.h"

