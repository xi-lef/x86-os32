#pragma once

#include "types.h"
//#include "device/cgastr.h"

class Time {
private:
    // Disallow copies and assignments.
    //Time(const Time&)            = delete; // TODO hm
    Time& operator=(const Time&) = delete;

public:
    /*
     * Indexing for these arrays starts at 1.
     */
    // TODO static?
    const char * const weekday_string[8] = {
        "Invalid", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    const char * const month_string[13] = {
        "Invalid", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    const uint8_t days_per_month_array[13] = {
        255, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    uint16_t second;
    uint16_t minute;
    uint16_t hour;
    uint16_t day;
    uint16_t month;
    uint16_t year;
    uint16_t weekday;
    uint16_t century;

    int8_t timezone;

    Time(int8_t timezone = 1) : timezone(timezone) {}

    /*
     * Updates the time.
     */
    void update_time();

// Converts hours to seconds. Useful for increment_seconds.
#define HOURS_TO_SECONDS(x) ((x) * 60 * 60)

    /*
     * Increments the seconds by amount.
     */
    void increment_seconds(uint32_t amount = 1);

    /*
     * Returns the days in a given month. This is necessary due to leap years.
     * month must be in [1, 12].
     */
    uint8_t days_per_month(uint8_t month);
};
