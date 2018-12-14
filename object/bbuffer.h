// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse BBuffer
 */

#pragma once

#include "types.h"

/*! \brief Die Klasse BBuffer implementiert einen "Bounded Buffer",
 *  also einen Puffer mit beschränkter Größe.
 *
 *  \tparam T gibt an welcher Typ gespeichert werden soll
 *  \tparam CAP gibt die Kapazität des Puffers an.
 */
template <typename T, unsigned CAP>
class BBuffer
{
	// Verhindere Kopien und Zuweisungen
	BBuffer(const BBuffer&)            = delete;
	BBuffer& operator=(const BBuffer&) = delete;

private:
	T data[CAP];
	volatile unsigned in;
	volatile unsigned out;

public:
	/*! \brief Der Konstruktor initialisiert den Puffer als leer.
	 *
	 */
	BBuffer() : in(0), out(0) {}

	/*! \brief In den Puffer einfügen.
	 *  \param val Das Element das eingefügt werden soll.
	 *  \return \b false wenn der Puffer voll ist und keine weiteres Element
	 *  mehr eingefügt werden kann, \b true sonst.
	 */
	bool produce(T val) {
		unsigned nextin = (in + 1) % CAP;
		if(nextin != out) {
			data[in] = val;
			in = nextin;
			return true;
		}
		return false;
	}

	/*! \brief Aus dem Puffer herausnehmen.
	 *  \param val Falls es noch Elemente im Puffer gibt wird das Nächste in
	 *  \b val gespeichert, ansonsten wird \b val nicht geschrieben.
	 *  \return \b false wenn der Puffer leer ist, \b true sonst.
	 */
	bool consume(T &val) {
        unsigned cur_in;
        unsigned new_out;
        bool ret = false;

        do {
            cur_in = in;
            if(cur_in != out) {
                val = data[out];
                new_out = (out + 1) % CAP; // if CAS fails, we must not change out.
                ret = true;
            }
        } while (__sync_bool_compare_and_swap(&in, cur_in, cur_in) == false);
        if (ret) {
            out = new_out;
        }
        return ret;
        /*if(in != out) {
            val = data[out];
            out = (out + 1) % CAP;
            return true;
        }
        return false;*/
    }
};

