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
    return *this << (b ? "true" : "false");
}

O_Stream& O_Stream::operator <<(short ival) {
    return *this << (long) ival;
}

O_Stream& O_Stream::operator <<(unsigned short ival) {
    return *this << (unsigned long) ival;
}

O_Stream& O_Stream::operator <<(int ival) {
    return *this << (long) ival;
}

O_Stream& O_Stream::operator <<(unsigned int ival) {
    return *this << (unsigned long) ival;
}

O_Stream& O_Stream::operator <<(long ival) {
    if (base == 10 && ival < 0) {
        put('-');
        ival = -ival;
    }
    return *this << (unsigned long) ival;
}

O_Stream& O_Stream::operator <<(unsigned long ival) {
    switch (base) {
    case 2:
        put('0'); put('b');
        break;
    case 8:
        put('0');
        break;
    case 10:
        break;
    case 16:
        put('0'); put('x');
        break;
    }

    unsigned long div; // biggest power of base thats smaller than ival
    for (div = 1; ival / div >= (unsigned long) base; div *= base);

    for (; div > 0; div /= (unsigned long) base) {
        char digit = ival / div;
        put(digit + ((digit < 10) ? '0' : ('a' - 10)));

        ival %= div;
    }

    return *this;
}

O_Stream& O_Stream::operator <<(const void* ptr) {
    int base_save = base;
    base = 16;
    *this << (unsigned long) ptr;
    base = base_save;
	
	return *this;
}

O_Stream& O_Stream::operator <<(O_Stream& (*f) (O_Stream&)) {
	return f(*this);
}

O_Stream& O_Stream::operator <<(Time& t) {
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
