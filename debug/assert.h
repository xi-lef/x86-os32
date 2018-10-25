// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält ein Makro um Zusicherungen (asserts) zu geben.
 *
 *  Sollte eine Zusicherung fehlschlagen, so wird eine Meldung mit Details auf
 *  der Debugausgabe getätigt und die entsprechende CPU angehalten.
 */

#pragma once

#ifndef STRINGIFY
/*!
 *  \def STRINGIFY(S)
 *  \brief Wandle den Makroparameter in eine Zeichenkette um
 *  \ingroup debug
 *  \param S Umzuwandelnder Ausdruck
 *  \return Ausdruck als Zeichenkette
 */
#define STRINGIFY(S) #S
#endif

/*!
 *  \def assert_size(TYPE, SIZE)
 *  \brief Prüfe zur Übersetzungszeit, ob ein Datentyp (oder Variable) die
 *  vorgegebene statische Speichergröße besitzt
 *  \ingroup debug
 *  \param TYPE zu prüfender Datentyp
 *  \param SIZE erwartete Speichergröße in Bytes
 */
#define assert_size(TYPE, SIZE) \
	static_assert(sizeof(TYPE) == (SIZE), "Wrong size for " STRINGIFY(TYPE))

/*!
 *  \def assert(EXP)
 *  \brief Prüfe, ob ein Ausdruck korrekt ist, oder gib eine Fehlermeldung aus
 *  und stoppe die CPU
 *  \ingroup debug
 *  \param EXP zugesicherter (zu prüfender) Ausdruck
 */
#ifdef NDEBUG
#define assert(EXP)
#else
#define assert(EXP) \
	do { \
		if (__builtin_expect(!(EXP), 0)) { \
			DBG << "Assertion '" << STRINGIFY(EXP) << "' failed (" << __func__ \
			    << " @ " << __FILE__ << ":" << __LINE__ << ") - CPU stopped." \
			    << endl; \
			CPU::die(); \
			while(1); \
		} \
	} while(0)

// Includes am Ende, damit die Makros auch darin verwendet werden könnten
#include "debug/output.h"
#include "machine/cpu.h"
#endif

