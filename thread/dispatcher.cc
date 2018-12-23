// vim: set et ts=4 sw=4:

#include "thread/dispatcher.h"


// TODO ?
void Dispatcher::kickoff(Thread *object) {
    object->action();
    for (;;) ;
}
