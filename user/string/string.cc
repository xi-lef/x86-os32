#include "user/string/string.h"
#include "debug/output.h"
#include "utils/math.h"

String::String() : len(0) {}

String::String(char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        data[i] = s[i];
    }   
    len = i;
}

String::operator char*() {
    for (int i = len; i < STRING_MAX_LENGTH; i++) {
        data[i] = '\0';
    }
    return data;
}

char String::at(size_t i) const {
    if (i >= len) {
        DBG << "String: at: invalid index" << endl;
    }
    return data[i];
}

char& String::operator [](size_t i) {
    if (i >= len) {
        DBG << "String: []: invalid index" << endl;
    }
    return data[i];
}

const char& String::operator [](size_t i) const {
    if (i >= len) {
        DBG << "String: []: invalid index" << endl;
    }
    return data[i];
}

String String::from(size_t pos) const {
    if (pos == 0) {
        return *this;
    }

    String new_str(*this);
    for (size_t i = 0; i < this->length() - pos; i++) {
        new_str[i] = new_str[i + pos];
    }
    new_str.len = Math::max(new_str.len - pos, (size_t) 0);
    return new_str;
}

String String::to(size_t pos) const {
    if (pos >= len) {
        return *this;
    }
    
    String new_str(*this);
    new_str.len = pos + 1;
    return new_str;
}

void String::resize(size_t n, char c) {
    if (n > len) {
        for (size_t i = len; i < n; i++) {
            data[i] = c;
        }
    }
    len = n;
}

size_t String::length() const {
    return len;
}

String String::without_lf() const {
    String new_str(*this);
    if (new_str[len - 1] == '\n') {
        new_str[len - 1] =  '\0';
        new_str.len--;
    }
    return new_str;
}

void String::remove_lf() {
    if (data[len - 1] == '\n') {
        data[len - 1] =  '\0';
        len--;
    }
}

bool String::append(char c) {
    if (len == STRING_MAX_LENGTH) {
        return false;
    }
    data[len++] = c;
    return true;
}

bool String::append(const String& s) {
    for (size_t i = 0; i < s.length(); i++) {
        if (!append(s[i])) {
            return false;
        }
    }
    return true;
}

String& String::operator +=(char c) {
    append(c);
    return *this;
}

String& String::operator +=(const String& s) {
    append(s);
    return *this;
}

bool String::push_back(char c) {
    return append(c);
}

bool String::pop_back() {
    if (len > 0) {
        resize(len - 1);
        return true;
    }
    return false;
}

void String::empty() {
    len = 0;
}

bool String::is_empty() const {
    return len == 0;
}

bool streq(String s1, String s2) {
    if (s1.length() != s2.length()) {
        return false;
    }
    for (size_t i = 0; i < s1.length(); i++) {
        if (s1[i] != s2[i]) {
            return false;
        }
    }
    return true;
}

long strtol(const String& s, bool *error, int base) {
    // initialize error for proper error handling
    if (error) {
        *error = false;
    }

    // skip leading whitespace
    size_t i;
    for (i = 0; s[i] == ' '; i++);

    // check if the number is negative
    bool negative = false;
    if (s[i] == '-') {
        negative = true;
        i++;
    }

    // check for prefixes for binary, octal, or hexadecimal.
    // check hexadecimal before octal, or else we would mistake 0x as octal!
    if ((base == 0 || base == 2)
        && s[i] == '0' && (s[i + 1] == 'b' || s[i + 1] == 'B')) {
        i += 2;
        base = 2;
    } else if ((base == 0 || base == 16)
        && s[i] == '0' && (s[i + 1] == 'x' || s[i + 1] == 'X')) {
        i += 2;
        base = 16;
    } else if ((base == 0 || base == 8)
        && s[i] == '0') {
        i += 1;
        base = 8;
    } else {
        base = 10;
    }

    // do the actual conversion
    unsigned long res = 0;
    for (; i < s.length(); i++) {
        char cur = s[i];
        unsigned long tmp = res;

        // take min(base, 10) or else we would accept "digits" like ';'
        if (cur >= '0' && cur <= ('0' + Math::min(base, 10) - 1)) {
            res = base * res + (cur - '0');
        } else if (base == 16 && ((cur >= 'a' && cur <= 'f')
                                  || (cur >= 'A' && cur <= 'F'))) {
            bool small = cur >= 'a' && cur <= 'f';
            res = base * res + (10 + cur - (small ? 'a' : 'A'));
        } else {
            if (error) {
                *error = true;
            }
            break;
        }

        if (res < tmp) {
            goto overflow;
        }
    }

    long ret;
    if (__builtin_add_overflow(res, 0, &ret)) {
        goto overflow;
    }

    return negative ? ret * -1 : ret;

overflow:
    if (error) {
        *error = true;
    }
    return SSIZE_MAX; // meh
}
