// vim: set et ts=4 sw=4:

#include "strbuf.h"
#include <stdio.h>

void Stringbuffer::put(char c) {
	buffer[pos++] = c;
	//printf("pos: %d\n", pos);
	if (pos == 80) {
		flush();
	}
}
