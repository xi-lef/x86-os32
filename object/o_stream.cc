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
	if (b == true) {
		put('1');
	} else {
		put('0');
	}
	return *this;
}

O_Stream& O_Stream::operator << (short ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
			break;		
		case 8:
			put('0');
			log2 = 3;
			break;
		case 10: {
			signed char r = ival % base;
			if (ival / base != 0) {
				*this << (ival / base);
			} else if (r < 0) {
				put('-');
			}
			
			put(r + 48); // +48 = ascii stuff
		} return *this;
		case 16:
			put('0'); put('x');
			log2 = 4;
			break;
	}
	
	for (int i = 15; i >= 0; i -= log2) {
		unsigned char cur = (ival >> i) & bit_base;
		if (cur != 0) {
			print = true;
		}
		if (print) {
			if (cur < 10) {
				put(((ival >> i) & bit_base) + 48); // +48 = ascii stuff
			} else {
				put(((ival >> i) & bit_base) + 55); // -10+65
			}
		}
	}
	return *this;
}

O_Stream& O_Stream::operator << (unsigned short ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
			break;		
		case 8:
			put('0');
			log2 = 3;
			break;
		case 10: {
			signed char r = ival % base;
			if (ival / base != 0) {
				*this << (ival / base);
			} else if (r < 0) {
				put('-');
			}
			
			put(r + 48); // +48 = ascii stuff
		} return *this;
		case 16:
			put('0'); put('x');
			log2 = 4;
			break;
	}
	
	for (int i = 15; i >= 0; i -= log2) {
		unsigned char cur = (ival >> i) & bit_base;
		if (cur != 0) {
			print = true;
		}
		if (print) {
			if (cur < 10) {
				put(((ival >> i) & bit_base) + 48); // +48 = ascii stuff
			} else {
				put(((ival >> i) & bit_base) + 55); // -10+65
			}
		}
	}
	return *this;
}

O_Stream& O_Stream::operator << (int ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
			break;		
		case 8:
			put('0');
			log2 = 3;
			break;
		case 10: {
			signed char r = ival % base;
			if (ival / base != 0) {
				*this << (ival / base);
			} else if (r < 0) {
				put('-');
			}
			
			put(r + 48); // +48 = ascii stuff
		} return *this;
		case 16:
			put('0'); put('x');
			log2 = 4;
			break;
	}
	
	for (int i = 31; i >= 0; i -= log2) {
		unsigned char cur = (ival >> i) & bit_base;
		if (cur != 0) {
			print = true;
		}
		if (print) {
			if (cur < 10) {
				put(((ival >> i) & bit_base) + 48); // +48 = ascii stuff
			} else {
				put(((ival >> i) & bit_base) + 55); // -10+65
			}
		}
	}
	return *this;
}

O_Stream& O_Stream::operator << (unsigned int ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
			break;		
		case 8:
			put('0');
			log2 = 3;
			break;
		case 10: {
			signed char r = ival % base;
			if (ival / base != 0) {
				*this << (ival / base);
			} else if (r < 0) {
				put('-');
			}
			
			put(r + 48); // +48 = ascii stuff
		} return *this;
		case 16:
			put('0'); put('x');
			log2 = 4;
			break;
	}
	
	for (int i = 31; i >= 0; i -= log2) {
		unsigned char cur = (ival >> i) & bit_base;
		if (cur != 0) {
			print = true;
		}
		if (print) {
			if (cur < 10) {
				put(((ival >> i) & bit_base) + 48); // +48 = ascii stuff
			} else {
				put(((ival >> i) & bit_base) + 55); // -10+65
			}
		}
	}
	return *this;
}

O_Stream& O_Stream::operator << (long ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
			break;		
		case 8:
			put('0');
			log2 = 3;
			break;
		case 10: {
			signed char r = ival % base;
			if (ival / base != 0) {
				*this << (ival / base);
			} else if (r < 0) {
				put('-');
			}
			
			put(r + 48); // +48 = ascii stuff
		} return *this;
		case 16:
			put('0'); put('x');
			log2 = 4;
			break;
	}
	
	for (int i = 31; i >= 0; i -= log2) {
		unsigned char cur = (ival >> i) & bit_base;
		if (cur != 0) {
			print = true;
		}
		if (print) {
			if (cur < 10) {
				put(((ival >> i) & bit_base) + 48); // +48 = ascii stuff
			} else {
				put(((ival >> i) & bit_base) + 55); // -10+65
			}
		}
	}
	return *this;
}

O_Stream& O_Stream::operator << (unsigned long ival) {
	bool print = false;                // set to true when the first character thats not equal to '0' appears
	unsigned char log2;                // log_2 of base
	unsigned char bit_base = base - 1; // used for bitwise ANDs
	switch (base) {
		case 2:
			put('0'); put('b');
			log2 = 1;
			break;		
		case 8:
			put('0');
			log2 = 3;
			break;
		case 10: {
			signed char r = ival % base;
			if (ival / base != 0) {
				*this << (ival / base);
			} else if (r < 0) {
				put('-');
			}
			
			put(r + 48); // +48 = ascii stuff
        } return *this;
		case 16:
			put('0'); put('x');
			log2 = 4;
			break;
	}
	
	for (int i = 31; i >= 0; i -= log2) {
		unsigned char cur = (ival >> i) & bit_base;
		if (cur != 0) {
			print = true;
		}
		if (print) {
			if (cur < 10) {
				put(((ival >> i) & bit_base) + 48); // +48 = ascii stuff
			} else {
				put(((ival >> i) & bit_base) + 55); // -10+65
			}
		}
	}
	return *this;
}

O_Stream& O_Stream::operator << (const void* ptr) {
    long ptr_long = long(ptr); // TODO
	put('0'); put('x');
	for (int i = 31; i >= 0; i -= 4) {
		unsigned int tmp = (ptr_long >> i) & 0xf;
		if (tmp < 10) {
			put(tmp + 48); // +48  = ascii stuff
		} else {
			put(tmp + 55); // -10+65
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
