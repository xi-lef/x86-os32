// vim: set et ts=4 sw=4:

#include "strbuf.h"

void Stringbuffer::put(char c) {
	buffer[pos++] = c;
	if (pos == 80) {
		flush();
	}
}
