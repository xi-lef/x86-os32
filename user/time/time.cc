#include "user/time/time.h"
#include "debug/output.h"
#include "machine/cpu.h"
#include "utils/math.h"
#include "device/cgastr.h"

void Time::set_timezone(int16_t zone) {
    timezone = zone;
}

void Time::apply_timezone() {
    if (timezone > 0) {
        increment_seconds(HOURS_TO_SECONDS(timezone));
    } else {
        decrement_seconds(HOURS_TO_SECONDS(-timezone));
    }
}

// might be overkill xd
void Time::increment_seconds(uint32_t amount) {
    if (second + amount < second) {
        DBG << "Time: overflow" << endl;
        return;
    }

    // unsigned underflow will happen if amount < 60, but thats fine
    for (; second + amount >= 60; amount -= 60) {
        if ((++minute %= 60) != 0) continue;
        if ((++hour %= 24) != 0) continue;

        weekday = weekday % 7 + 1;
        if ((++day %= get_days_per_month(month, year, century) + 1) != 0) continue;
        day++; // Day is 0, so increment it for 1-indexing.

        if ((++month %= 13) != 0) continue;
        month++; // Month is 0, so increment it for 1-indexing.

        if ((++year %= 100) != 0) continue;
        if ((++century %= 100) == 0) DBG << "Time: Nice cheats" << endl;
    }

    second += amount;
}

void Time::decrement_seconds(int32_t amount) {
    assert(amount > 0);
    // underflow cant happen because second is always >= 0

    for (int32_t sec = (int32_t) second; sec - amount < 0; amount -= 60) {
        minute = (minute == 0) ? 59 : minute - 1;
        if (minute != 59) continue;

        hour = (hour == 0) ? 23 : hour - 1;
        if (hour != 23) continue;

        weekday = (weekday == 1) ? 7 : weekday - 1;

        if (day != 1) {
            day--;
            continue;
        }
        // we need to change month first, so get_days_per_month is correct
        month = (month == 1) ? 12 : month - 1;
        // year/century might be wrong, but that doesnt matter
        day = get_days_per_month(month, year, century);
        if (month != 12) continue;

        year = (year == 0) ? 99 : year - 1;
        if (year != 99) continue;

        if (century == 0) DBG << "Time: Nice cheats" << endl;
        century--;
    }

    second -= amount;
}

Time& Time::operator +=(unsigned int i) {
    increment_seconds(i);
    return *this;;
}

// Prefix
Time& Time::operator ++() {
    increment_seconds(1);
    return *this;
}

// Postfix
Time Time::operator ++(int) {
    Time ans(*this);
    increment_seconds(1);
    return ans;
}

Time& Time::operator -=(int i) {
    decrement_seconds(i);
    return *this;;
}

// Prefix
Time& Time::operator --() {
    decrement_seconds(1);
    return *this;
}

// Postfix
Time Time::operator --(int) {
    Time ans(*this);
    decrement_seconds(1);
    return ans;
}

// This is necessary due to C++11. :(
constexpr const char *Time::weekday_string[8];
constexpr const char *Time::month_string[13];
constexpr uint16_t Time::days_per_month[13];

const char *Time::get_weekday_string(uint16_t weekday) {
    return weekday_string[(weekday < 8) ? weekday : 0];
}

const char *Time::get_month_string(uint16_t month) {
    return month_string[(month < 13) ? month : 0];
}

uint16_t Time::get_days_per_month(uint16_t month, uint16_t year, uint16_t century) {
    // Is it february in a leap year?
    // Keep in mind how the RTC stores the year: it uses year and century.
    if (month == 2 && year % 4 == 0 && (year != 0 || century % 4 == 0)) {
        return days_per_month[month] + 1; // = 29
    }
    return days_per_month[(month < 13) ? month : 0];
}
