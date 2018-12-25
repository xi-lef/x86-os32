// vim: set et ts=4 sw=4:

#include "thread/dispatcher.h"
#include "debug/output.h"


// TODO ?
void Dispatcher::kickoff(Thread *object) {
    DBG << "kickoff " << flush;
    object->action();
    DBG << "hm " << flush;
    for (;;) ;
}
