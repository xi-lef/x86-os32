#include "user/time/time.h"
#include "debug/output.h"
#include "machine/cpu.h"
#include "utils/math.h"
#include "device/cgastr.h"

// might be overkill xdd
void Time::increment_seconds(uint32_t amount) {
    while (amount--) {
        if ((++second %= 60) != 0) continue;
        if ((++minute %= 60) != 0) continue;
        if ((++hour %= 24) != 0) continue;

        weekday = weekday % 7 + 1;
        if ((++day %= get_days_per_month(month, year) + 1) != 0) continue;
        day++; // day is 0, so increment it for 1-indexing
        if ((++month %= 12 + 1) != 0) continue;
        month++; // month is 0, so increment it for 1-indexing
        if ((++year %= 100) != 0) continue;
        if ((++century %= 100) != 0) DBG << "very funny." << endl;
    }
}

Time& Time::operator+(unsigned int i) {
    increment_seconds(i);
    return *this;
}

// this is necessary due to C++11 :(
const char * const Time::weekday_string[];
const char * const Time::month_string[];
const uint16_t Time::days_per_month[];

const char *Time::get_weekday_string(uint16_t weekday) {
    return weekday_string[weekday];
}

const char *Time::get_month_string(uint16_t month) {
    return month_string[month];
}

uint16_t Time::get_days_per_month(uint16_t month, uint16_t year) {
    if (month == 2 && year % 4 == 0) { // is it february in a leap year?
        return days_per_month[month] + 1; // = 29
    }
    return days_per_month[month];
}
