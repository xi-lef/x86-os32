#include "user/time/time.h"
#include "debug/output.h"
#include "machine/cpu.h"
#include "utils/math.h"
#include "device/cgastr.h"

// might be overkill xdd
void Time::increment_seconds(uint32_t amount) {
    uint32_t next = second + amount;
    if (next < second) {
        DBG << "Time: overflow" << endl;
        return;
    }

    while (next >= 60) {
        next -= 60;
        if ((++minute %= 60) != 0) continue;
        if ((++hour %= 24) != 0) continue;

        weekday = weekday % 7 + 1;
        if ((++day %= get_days_per_month(month, year, century) + 1) != 0) continue;
        day++; // day is 0, so increment it for 1-indexing
        if ((++month %= 12 + 1) != 0) continue;
        month++; // month is 0, so increment it for 1-indexing
        if ((++year %= 100) != 0) continue;
        if ((++century %= 100) == 0) DBG << "Time: Nice cheats" << endl;
    }
    second = next;
}

Time& Time::operator +=(unsigned int i) {
    increment_seconds(i);
    return *this;;
}

// prefix
Time& Time::operator ++() {
    increment_seconds(1);
    return *this;
}

// postfix
Time Time::operator ++(int) {
    Time ans = *this;
    increment_seconds(1);
    return ans;
}

// this is necessary due to C++11 :(
constexpr const char * const Time::weekday_string[];
constexpr const char * const Time::month_string[];
constexpr const uint16_t Time::days_per_month[];

const char *Time::get_weekday_string(uint16_t weekday) {
    return weekday_string[weekday];
}

const char *Time::get_month_string(uint16_t month) {
    return month_string[month];
}

uint16_t Time::get_days_per_month(uint16_t month, uint16_t year, uint16_t century) {
    // is it february in a leap year?
    // keep in mind how the RTC stores the year: it uses year and century.
    if (month == 2 && year % 4 == 0 && (year != 0 || century % 4 == 0)) {
        return days_per_month[month] + 1; // = 29
    }
    return days_per_month[month];
}
