#include "user/string/string.h"
#include "debug/output.h"
#include "utils/math.h"
#include "utils/memutil.h"

String::String() : len(0), save_index(0) {}

String::String(char *s) : save_index(0) {
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

String& String::operator =(const String& str) {
    strncpy(data, str.data, str.length());
    len = str.len;
    return *this;
}

char String::at(int i) const {
    if (i < 0 || (size_t) i >= len) {
        DBG << "String: at: invalid index" << endl;
    }
    return data[i];
}

char& String::operator [](int i) {
    if (i < 0 || (size_t) i >= len) {
        DBG << "String: []: invalid index" << endl;
    }
    return data[i];
}

const char& String::operator [](int i) const {
    if (i < 0 || (size_t) i >= len) {
        DBG << "String: []: invalid index" << endl;
    }
    return data[i];
}

bool String::operator ==(const String& str) const {
    return streq(*this, str);
}

String String::from(size_t pos) const {
    if (pos == 0) {
        return *this;
    } else if (pos > len) {
        return String();
    }

    String new_str(*this);
    for (size_t i = 0; i < this->length() - pos; i++) {
        new_str[i] = new_str[i + pos];
    }
    new_str.len -= pos;
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

bool String::append(const String& str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (!append(str[i])) {
            return false;
        }
    }
    return true;
}

String& String::operator +=(char c) {
    append(c);
    return *this;
}

String& String::operator +=(const String& str) {
    append(str);
    return *this;
}

String& String::insert(size_t pos, char c) {
    String end = from(pos);
    resize(pos);
    append(c);
    append(end);
    return *this;
}

String& String::insert(size_t pos, const String& str) {
    String end = from(pos);
    resize(pos);
    append(str);
    append(end);
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

bool String::compare(size_t pos, const String& str) const {
    if (pos + str.length() > len) {
        return false;
    }

    for (size_t i = pos; i < len; i++) {
        if (data[i] != str[i - pos]) {
            return false;
        }
    }
    return true;
}

size_t String::find_first_of(char c, size_t pos) const {
    for (size_t i = pos; i < len; i++) {
        if (data[i] == c) {
            return i;
        }
    }
    return -1;
}

size_t String::find_first_of(const String& str, size_t pos) const {
    for (size_t i = pos; i < len; i++) {
        for (size_t j = 0; j < str.length(); j++) {
            if (data[i] == str[j]) {
                return i;
            }
        }
    }
    return -1;
}

String String::tok(const String& delim) {
    return strtok(*this, delim);
}

void String::empty() {
    len = 0;
}

bool String::is_empty() const {
    return len == 0;
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*p != '\0') {
        p++;
    }
    return p - s;
}

char *strcpy(char *dest, const char *src) {
    return strncpy(dest, src, strlen(src));
}

char *strncpy(char *dest, const char *src, size_t size) {
    return (char *) memcpy(dest, src, size);
}

bool streq(const String& str1, const String& str2) {
    if (str1.length() != str2.length()) {
        return false;
    }
    for (size_t i = 0; i < str1.length(); i++) {
        if (str1[i] != str2[i]) {
            return false;
        }
    }
    return true;
}

String strtok(String& str, const String& delim) {
    size_t pos;
    // skip leading delimiting chars
    while ((pos = str.find_first_of(delim, str.save_index)) == str.save_index) {
        str.save_index++;
    }
    if (pos == (size_t) -1) {
        // no delimiting char could be found
        pos = str.length();
    }

    String ret = str.to(pos - 1).from(str.save_index);
    str.save_index = pos + 1;
    return ret;
}

long strtol(const String& str, bool *error, int base) {
    // initialize error for proper error handling
    if (error) {
        *error = false;
    }

    // skip leading whitespace
    size_t i;
    for (i = 0; i < str.length() && str[i] == ' '; i++);

    // check if the number is negative
    bool negative = false;
    if (i < str.length() && str[i] == '-') {
        negative = true;
        i++;
    }

    // check for prefixes for binary, octal, or hexadecimal.
    // check hexadecimal before octal, or else we would mistake 0x as octal!
    if ((base == 0 || base == 2) && (i + 1) < str.length()
        && str[i] == '0' && (str[i + 1] == 'b' || str[i + 1] == 'B')) {
        i += 2;
        base = 2;
    } else if ((base == 0 || base == 16) && (i + 1) < str.length()
        && str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X')) {
        i += 2;
        base = 16;
    } else if ((base == 0 || base == 8) && i < str.length()
        && str[i] == '0') {
        i += 1;
        base = 8;
    } else if (base == 0) {
        base = 10;
    }

    // do the actual conversion
    unsigned long res = 0;
    for (; i < str.length(); i++) {
        char cur = str[i];
        unsigned long tmp = res;

        // take min(base, 10) or else we would accept "digits" like ';'
        if (cur >= '0' && cur <= ('0' + Math::min(base, 10) - 1)) {
            res = base * res + (cur - '0');
        } else if (base > 10 && ((cur >= 'a' && cur <= 'a' + base)
                                 || (cur >= 'A' && cur <= 'A' + base))) {
            bool small = cur >= 'a' && cur <= 'a' + base;
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

    if (res > SSIZE_MAX) {
        goto overflow;
    }
    return negative ? (long) res * -1 : res;

overflow:
    if (error) {
        *error = true;
    }
    return SSIZE_MAX; // meh
}
