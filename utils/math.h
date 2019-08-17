// vim: set et ts=4 sw=4:

#pragma once

#include "types.h"

namespace Math {
	/*! \brief Berechnet die Ganzzahl-Division von \b num durch \b den mit
	 * 64-Bit-Genauigkeit und schreibt den Rest der Division in \b rem_p, falls
	 * \b rem_p nicht \b NULL ist.
	 *
	 */
	inline uint64_t div64(uint64_t num, uint64_t den, uint64_t *rem_p = 0) {
		uint64_t quot = 0, qbit = 1;
		
		while((int64_t)den >= 0) {
			den <<= 1;
			qbit <<= 1;
		}
		
		while(qbit) {
			if(den <= num) {
				num -= den;
				quot += qbit;
			}
			den >>= 1;
			qbit >>= 1;
		}
		
		if(rem_p) {
			*rem_p = num;
		}
		return quot;
	}

	template <typename T, typename U>
	inline T min(T a, U b)
	{
		return a > b ? b : a;
	}

	template <typename T, typename U>
	inline T max(T a, U b)
	{
		return a > b ? a : b;
	}

    inline uint32_t pow(uint32_t base, uint32_t exp) {
        uint32_t result = 1;
        for (;;) {
            if (exp & 1) {
                result *= base;
            }
            exp >>= 1;
            if (!exp) {
                break;
            }
            base *= base;
        }
        return result;
    }

    /*
     * Binary Coded Decimal to Integer conversion (and vice versa).
     * Assume an 8 bit BCD, so only values from 0 to 99 are supported.
     */
    inline uint16_t bcd_to_int(uint8_t bcd) {
        return (bcd >> 4) * 10 + (bcd & 0xf);
    }

    inline uint16_t int_to_bcd(uint8_t i) {
        return (i / 10 << 4) + i % 10;
    }
};

