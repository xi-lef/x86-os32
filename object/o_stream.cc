// vim: set et ts=4 sw=4:

#include "o_stream.h"

O_Stream& O_Stream::operator << (char c) {
	put(c);
	return *this;
}

O_Stream& O_Stream::operator << (unsigned char c) {
	put(c);
	return *this;
}

O_Stream& O_Stream::operator << (const char* string) {
	for (const char *c = string; *c != '\0'; c++) {
		put(*c);
	}
	return *this;
}

O_Stream& O_Stream::operator << (bool b) {
    put(b ? '1' : '0');
	return *this;
}

O_Stream& O_Stream::operator << (short ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                unsigned char cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + 48); // ascii stuff
                }
            }
            return *this;
        case 10: 
            {
                signed char r = ival % base;
                if (ival / base != 0) {
                    *this << (ival / base);
                } else if (r < 0) {
                    put('-');
                }

                if (r < 0) {
                    r *= -1;
                }
                put(r + 48); // ascii stuff
            } return *this;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) { // could be "i = 32 - log2" to safe computing time TODO
        unsigned char cur = (ival >> i) & bit_base;
        //printf("ival = %d\ti = %d\tcur = %d\n", ival, i, cur);
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? 48 : 87)); // ascii stuff
        }
    }
    return *this;
}

O_Stream& O_Stream::operator << (unsigned short ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                unsigned char cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + 48); // ascii stuff
                }
            }
            return *this;
        case 10: 
            {
                signed char r = ival % base;
                if (ival / base != 0) {
                    *this << (ival / base);
                }
				
                put(r + 48); // ascii stuff
            } return *this;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) { // could be "i = 32 - log2" to safe computing time TODO
        unsigned char cur = (ival >> i) & bit_base;
        //printf("ival = %d\ti = %d\tcur = %d\n", ival, i, cur);
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? 48 : 87)); // ascii stuff
        }
    }
    return *this;
}
#include <stdio.h>
O_Stream& O_Stream::operator << (int ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                unsigned char cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + 48); // ascii stuff
                }
            }
            return *this;
        case 10: 
            {
                signed char r = ival % base;
                if (ival / base != 0) {
                    *this << (ival / base);
                } else if (r < 0) {
                    put('-');
                }

                if (r < 0) {
                    r *= -1;
                }
                put(r + 48); // ascii stuff
            } return *this;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) { // could be "i = 32 - log2" to safe computing time TODO
        unsigned char cur = (ival >> i) & bit_base;
        //printf("ival = %d\ti = %d\tcur = %d\n", ival, i, cur);
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? 48 : 87)); // ascii stuff
        }
    }
    return *this;
}

O_Stream& O_Stream::operator << (unsigned int ival) {
    bool print = false;                // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                unsigned char cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + 48); // ascii stuff
                }
            }
            return *this;
        case 10: 
            {
                signed char r = ival % base;
                if (ival / base != 0) {
                    *this << (ival / base);
                }
				
                put(r + 48); // ascii stuff
            } return *this;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) { // could be "i = 32 - log2" to safe computing time TODO
        unsigned char cur = (ival >> i) & bit_base;
        //printf("ival = %d\ti = %d\tcur = %d\n", ival, i, cur);
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? 48 : 87)); // ascii stuff
        }
    }
    return *this;
}

O_Stream& O_Stream::operator << (long ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                unsigned char cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + 48); // ascii stuff
                }
            }
            return *this;
        case 10: 
            {
                signed char r = ival % base;
                if (ival / base != 0) {
                    *this << (ival / base);
                } else if (r < 0) {
                    put('-');
                }

                if (r < 0) {
                    r *= -1;
                }
                put(r + 48); // ascii stuff
            } return *this;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) { // could be "i = 32 - log2" to safe computing time TODO
        unsigned char cur = (ival >> i) & bit_base;
        //printf("ival = %d\ti = %d\tcur = %d\n", ival, i, cur);
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? 48 : 87)); // ascii stuff
        }
    }
    return *this;
}

O_Stream& O_Stream::operator << (unsigned long ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                unsigned char cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + 48); // ascii stuff
                }
            }
            return *this;
        case 10: 
            {
                signed char r = ival % base;
                if (ival / base != 0) {
                    *this << (ival / base);
                }
				
                put(r + 48); // ascii stuff
            } return *this;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) { // could be "i = 32 - log2" to safe computing time TODO
        unsigned char cur = (ival >> i) & bit_base;
        //printf("ival = %d\ti = %d\tcur = %d\n", ival, i, cur);
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? 48 : 87)); // ascii stuff
        }
    }
    return *this;
}

O_Stream& O_Stream::operator << (const void* ptr) {
    long ptr_long = long(ptr); // TODO
	put('0'); put('x');
	for (int i = 28; i >= 0; i -= 4) {
		unsigned int cur = (ptr_long >> i) & 0xf;
		if (cur < 10) {
			put(cur + 48); // +48  = ascii stuff
		} else {
			put(cur + 87); // -10+97
		}
	}
	
	return *this;
}

O_Stream& O_Stream::operator << (O_Stream& (*f) (O_Stream&)) {
	return f(*this);
}

O_Stream& flush(O_Stream& os) {
	os.flush();
	return os;
}

O_Stream& endl(O_Stream& os) {
	os << '\n';
    os.flush();
	return os;
}

O_Stream& bin(O_Stream& os) {
	os.base = 2;
	return os;
}

O_Stream& oct(O_Stream& os) {
	os.base = 8;
	return os;
}

O_Stream& dec(O_Stream& os) {
	os.base = 10;
	return os;
}

O_Stream& hex(O_Stream& os) {
	os.base = 16;
	return os;
}
