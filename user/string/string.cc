#include "user/string/string.h"
#include "debug/output.h"
#include "utils/math.h"
#include "utils/heap.h"
#include "utils/memutil.h"

#define used_data (use_heap ? data : short_data)

String::String() : use_heap(false), len(0), save_index(0) {}

String::String(char *s) : use_heap(false), len(0), save_index(0) {
    for (char *c = s; *c != '\0'; c++) {
        append(*c);
    }
}

String::String(const String& str) : use_heap(false), len(0), save_index(0) {
    append(str);
}

String::~String() {
    if (use_heap) {
        free(data);
    }
}

String& String::operator =(const String& str) {
    clear();
    append(str);
    save_index = 0;
    return *this;
}

String::operator const char*() {
    _append_nullbyte();
    return used_data;
}

char& String::at(int i) {
    assert(i >= 0);
    size_t index = (size_t)i;
    assert(index <= length());

    if (index == length()) {
        _append_nullbyte();
    }

    return used_data[index];
}

const char& String::at(int i) const {
    assert(i >= 0);
    size_t index = (size_t)i;
    assert(index <= length());

    if (index == length()) {
        assert(!"TODO");
        //_append_nullbyte(); // TODO
    }

    return used_data[index];
    //return ((String) *this).at(i); // TODO hm
}

char& String::operator [](int i) {
    return at(i);
}

const char& String::operator [](int i) const {
    return at(i);
}

bool String::operator ==(const String& str) const {
    return streq(*this, str);
}

size_t String::length() const {
    return len;
}

size_t String::capacity() const {
    return use_heap ? cap : sizeof(short_data);
}

void String::clear() {
    len = 0;
    save_index = 0;
}

bool String::empty() const {
    return length() == 0;
}

void String::resize(size_t n, char c) {
    if (n <= length()) {
        len = n;
        return;
    }

    reserve(n);
    for (size_t i = length(); i < n; i++) {
        append(c);
    }
}

void String::reserve(size_t n) {
    if (n <= capacity()) {
        return;
    }

    if (!use_heap) {
        // initial switch from "short string" to "long string".
        // use "tmp" so we can copy the string content.
        size_t new_cap = 2 * sizeof(short_data);
        char *tmp = (char*)malloc(new_cap);
        assert(tmp);
        copy(tmp, new_cap);

        data = tmp;
        cap = new_cap;
        use_heap = true;
    }

    while (cap < n) {
        cap *= 2;
        data = (char*)realloc(data, cap);
        assert(data);
    }
}

void String::shrink_to_fit() {
    if (!use_heap) {
        return;
    }

    if (length() <= SHORT_STRING_LENGTH) {
        char *tmp = data;
        strcpy(short_data, tmp);

        use_heap = false;
        free(tmp);
    } else {
        cap = length();
        data = (char*)realloc(data, cap);
        assert(data);
    }
}

String String::substr(size_t pos, size_t len) const {
    if (pos == 0 && len >= length()) {
        return *this;
    }
    if (pos >= length()) {
        return String();
    }

    // check for potential overflow
    size_t limit = (pos + len < pos) ? length() : Math::min(pos + len, length());

    String new_str;
    for (size_t i = pos; i < limit; i++) {
        new_str += at(i);
    }
    return new_str;
}

void String::_append(char c) {
    at(len++) = c;
}

void String::_append_nullbyte() {
    // '\0' requires one byte, but doesnt increase the length of the string
    reserve(length() + 1);

    // using at() would result in a loop, so we use the data directly
    used_data[length()] = '\0';
}

String& String::append(char c) {
    reserve(length() + 1);
    _append(c);
    return *this;
}

String& String::append(const String& str) {
    reserve(length() + str.length());
    for (size_t i = 0; i < str.length(); i++) {
        _append(str[i]);
    }
    return *this;
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
    if (pos > length()) {
        return *this;
    }
    String end = substr(pos);
    resize(pos);
    append(c);
    append(end);
    return *this;
}

String& String::insert(size_t pos, const String& str) {
    if (pos > length()) {
        return *this;
    }
    String end = substr(pos);
    resize(pos);
    append(str);
    append(end);
    return *this;
}

String& String::erase(size_t pos, size_t len) {
    if (pos >= length()) {
        return *this;
    }
    if (len == npos) {
        *this = substr(0, pos);
        return *this;
    }
    // catch overflow
    if (pos + len < pos) {
        return *this;
    }

    String new_str = substr(0, pos);   // 1st part
    new_str.append(substr(pos + len)); // 2nd part
    *this = new_str;
    return *this;
}

void String::push_back(char c) {
    append(c);
}

void String::pop_back() {
    if (length() > 0) {
        resize(length() - 1);
    }
}

String String::without_lf() const {
    String new_str(*this);
    if (new_str[length() - 1] == '\n') {
        new_str.pop_back();
    }
    return new_str;
}

void String::remove_lf() {
    if (at(length() - 1) == '\n') {
        pop_back();
    }
}

bool String::compare(size_t pos, const String& str) const {
    if (pos + str.length() > length()) {
        return false;
    }

    for (size_t i = pos; i < Math::min(length(), str.length() + pos); i++) {
        if (at(i) != str[i - pos]) {
            return false;
        }
    }
    return true;
}

size_t String::find(const String& str, size_t pos) const {
    for (size_t i = pos; i < length(); i++) {
        if (compare(i, str)) {
            return i;
        }
    }
    return npos;
}

size_t String::find_first_of(char c, size_t pos) const {
    for (size_t i = pos; i < length(); i++) {
        if (at(i) == c) {
            return i;
        }
    }
    return npos;
}

size_t String::find_first_of(const String& str, size_t pos) const {
    for (size_t i = pos; i < length(); i++) {
        for (size_t j = 0; j < str.length(); j++) {
            if (at(i) == str[j]) {
                return i;
            }
        }
    }
    return npos;
}

String String::tok(const String& delim) {
    return strtok(*this, delim);
}

size_t String::copy(char *s, size_t len, size_t pos) const {
    // we need to check pos, or weird things could happen in case of underflow
    if (pos >= length()) {
        return 0;
    }

    size_t i;
    for (i = 0; i < Math::min(len, length() - pos); i++) {
        s[i] = at(pos + i);
    }
    return i;
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
    return (char*)memcpy(dest, src, size);
}

int strcmp(const String& str1, const String& str2) {
    for (size_t i = 0; i < Math::min(str1.length(), str2.length()); i++) {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
    }

    if (str1.length() != str2.length()) {
        return (str1.length() > str2.length()) ? 1 : -1;
    }
    return 0;
}

bool streq(const String& str1, const String& str2) {
    return strcmp(str1, str2) == 0;
}

String strtok(String& str, const String& delim) {
    size_t pos;
    // skip leading delimiting chars
    while ((pos = str.find_first_of(delim, str.save_index)) == str.save_index) {
        str.save_index++;
    }
    if (pos == String::npos) {
        // no delimiting char could be found
        pos = str.length();
    }

    String ret = str.substr(str.save_index, pos - str.save_index);
    str.save_index = pos + 1;
    return ret;
}

long strtol(const String& str, bool *error, int base) {
    // initialize error in case of empty input
    if (error) {
        *error = true;
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
    // check octal last, or else we would mistake 0b or 0x as octal!
    if ((base == 0 || base == 2)
        && (str.compare(i, "0b") || str.compare(i, "0B"))) {
        i += 2;
        base = 2;
    } else if ((base == 0 || base == 16)
        && (str.compare(i, "0x") || str.compare(i, "0X"))) {
        i += 2;
        base = 16;
    } else if ((base == 0 || base == 8)
        && str.compare(i, "0") && str.length() > i + 1) {
        // the last check is to not detect a simple "0" as base 8
        i += 1;
        base = 8;
    } else if (base == 0) {
        base = 10;
    }

    // do the actual conversion
    unsigned long res = 0;
    for (; i < str.length(); i++) {
        // detect empty input as an error
        if (error) {
            *error = false;
        }

        char cur = str[i];
        unsigned long tmp = res;

        // take min(base, 10) or else we would accept "digits" like ';'
        if (cur >= '0' && cur <= ('0' + Math::min(base, 10) - 1)) {
            res = base * res + (cur - '0');
        } else if (base > 10 && ((cur >= 'a' && cur <= 'a' + base - 10 - 1)
                                 || (cur >= 'A' && cur <= 'A' + base - 10 - 1))) {
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
    return negative ? (long)res * -1 : res;

overflow:
    if (error) {
        *error = true;
    }
    return SSIZE_MAX; // meh
}
