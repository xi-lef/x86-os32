// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse IdleThread
 */

#include "thread/thread.h"

/*! \brief Thread, der immer dann läuft, wenn eine CPU nichts zu tun hat.
 *  \ingroup thread
 *
 *
 *  Die Verwendung von IdleThread vereinfacht die
 *  Behandlung von "däumchendrehenden" Prozessoren.
 *  \note
 *  Instanzen von IdleThread sollten nie in der Bereitliste des Schedulers
 *  auftauchen, sondern immer getrennt gehalten werden, da sie ja nur dann
 *  ausgeführt werden sollen, wenn kein normaler Thread mehr bereit ist.
 *
 */
class IdleThread : public Thread {
public:
    IdleThread(void *tos) : Thread(tos) {}

    void action();
};
