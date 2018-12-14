#include "user/time/time.h"
#include "debug/output.h"
#include "machine/cpu.h"
#include "utils/math.h"
#include "device/cgastr.h"

// might be overkill xdd
void Time::increment_seconds(uint32_t amount) {
    //second += amount;
    while (amount--) {
    //for (int i = 0; i < amount; i++) {
    //for (;;) {
        if ((++second %= 60) != 0) continue;
        if ((++minute %= 60) != 0) continue;
        if ((++hour %= 24) != 0) continue;

        weekday = weekday % 7 + 1;
        if ((++day %= days_per_month(month) + 1) != 0) continue;
        day++; // day is 0, so increment it for 1-indexing
        if ((++month %= 12 + 1) != 0) continue;
        month++; // month is 0, so increment it for 1-indexing
        if ((++year %= 100) != 0) continue;
        if ((++century %= 100) != 0) DBG << "very funny." << endl;
    }
}

uint8_t Time::days_per_month(uint8_t month) {
    if (month == 2 && year % 4 == 0) { // is it february in a leap year?
        return days_per_month_array[month] + 1; // = 29
    }
    return days_per_month_array[month];
}
