#include "types.h"

extern "C" int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle)
{
	(void)func;
	(void)arg;
	(void)dso_handle;

	return 0;
}

extern "C" void __cxa_pure_virtual()
{

}
