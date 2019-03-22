// vim: set et ts=4 sw=4:

#include "o_stream.h"
#include "types.h"
#include "debug/output.h"

O_Stream& O_Stream::operator <<(char c) {
	put(c);
	return *this;
}

O_Stream& O_Stream::operator <<(unsigned char c) {
	put(c);
	return *this;
}

O_Stream& O_Stream::operator <<(const char* string) {
	for (const char *c = string; *c != '\0'; c++) {
		put(*c);
	}
	return *this;
}

O_Stream& O_Stream::operator <<(bool b) {
    *this << (b ? "true" : "false");
	return *this;
}

O_Stream& O_Stream::operator <<(short ival) {
    put_signed(ival);
    return *this;
}

O_Stream& O_Stream::operator <<(unsigned short ival) {
    put_unsigned(ival);
    return *this;
}

O_Stream& O_Stream::operator <<(int ival) {
    put_signed(ival);
    return *this;
}

O_Stream& O_Stream::operator <<(unsigned int ival) {
    put_unsigned(ival);
    return *this;
}

O_Stream& O_Stream::operator <<(long ival) {
    put_signed(ival);
    return *this;
}

O_Stream& O_Stream::operator <<(unsigned long ival) {
    put_unsigned(ival);
    return *this;
}

void O_Stream::put_signed(long ival) {
    bool    print    = false;    // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	uint8_t log2     = 1;        // log_2 of base; = 1 because g++
	uint8_t bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                uint8_t cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + '0');
                }
            }
            return;
        case 10: {
            int8_t r = ival % base;
            if (ival / base != 0) {
                *this << (ival / base);
            } else if (r < 0) {
                put('-');
            }

            if (r < 0) {
                r *= -1;
            }
            put(r + '0');
        } return;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) {
        uint8_t cur = (ival >> i) & bit_base;
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? '0' : ('a' - 10)));
        }
    }

    if (!print) {
        put('0');
    }
}

void O_Stream::put_unsigned(unsigned long ival) {
    bool    print    = false;    // set to true when the first character thats not equal to '0' appears, so that leading 0s dont get printed
	uint8_t log2     = 1;        // log_2 of base; = 1 because g++
	uint8_t bit_base = base - 1; // used for bitwise ANDs
    
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
            break;
        case 8: // octal is a bitch
            put('0');
			log2 = 3;

            for (int i = 30; i >= 0; i -= log2) {
                uint8_t cur = (ival >> i) & ((i == 30) ? 0x3 : bit_base); // to avoid sign extension on first digit
                if (cur != 0) {
                    print = true;
                }
                if (print) {
                    put(cur + '0');
                }
            }
            return;
        case 10: {
            uint8_t r = ival % base;
            if (ival / base != 0) {
                *this << (ival / base);
            }

            put(r + '0');
        } return;
        case 16:
            put('0'); put('x');
            log2 = 4;
            break;
    }

    // for binary or hexadecimal
    for (int i = 32 - log2; i >= 0; i -= log2) {
        uint8_t cur = (ival >> i) & bit_base;
        if (cur != 0) {
            print = true;
        }
        if (print) {
            put(cur + ((cur < 10) ? '0' : ('a' - 10)));
        }
    }

    if (!print) {
        put('0');
    }
}

O_Stream& O_Stream::operator <<(const void* ptr) {
    intptr_t ptr_long = intptr_t(ptr);
    int save_base = base;

    base = 16;
    *this << ptr_long;
    base = save_base;
	
	return *this;
}

O_Stream& O_Stream::operator <<(O_Stream& (*f) (O_Stream&)) {
	return f(*this);
}

O_Stream& O_Stream::operator <<(Time t) {
    *this << Time::get_weekday_string(t.weekday)
          << ((t.day     < 10) ? "  " : " ") << t.day
          << " " << t.get_month_string(t.month)
          << ((t.century < 10) ? "  " : " ") << t.century
          << ((t.year    < 10) ? "0"  : "")  << t.year << ","
          << ((t.hour    < 10) ? " 0" : " ") << t.hour
          << ((t.minute  < 10) ? ":0" : ":") << t.minute
          << ((t.second  < 10) ? ":0" : ":") << t.second;

    return *this;
}

O_Stream& O_Stream::operator <<(CGA_Screen::Attribute& attr) {
    (void) attr;
    DBG << "No colors supported!" << endl;
    return *this;
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
