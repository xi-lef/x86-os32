// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Assassin
 */

#pragma once

#include "guard/gate.h"

class Assassin : public Gate {
    Assassin(const Assassin&)            = delete;
    Assassin& operator=(const Assassin&) = delete;

public:
    Assassin() {}

    void hire();

    bool prologue();

    void epilogue();
};

extern Assassin assassin;
