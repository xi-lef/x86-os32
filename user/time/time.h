#pragma once

#include "types.h"

class Time {
private:
    //Time(const Time&)            = delete;
    //Time& operator=(const Time&) = delete;

    /*
     * Indexing for these arrays should start at 1.
     */
    constexpr static const char *weekday_string[8] = {
        "???", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    constexpr static const char *month_string[13] = {
        "???", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    constexpr static uint16_t days_per_month[13] = {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

public:
    uint16_t second;
    uint16_t minute;
    uint16_t hour;
    uint16_t day;
    uint16_t month;
    uint16_t year;
    uint16_t weekday;
    uint16_t century;

    int16_t timezone;

    Time(int16_t timezone = 1) : timezone(timezone) {}

// Converts hours to seconds. Useful for increment_seconds.
#define HOURS_TO_SECONDS(x) ((x) * 60 * 60)

    /*
     * Increments the seconds by amount.
     * If you want to call this from anywhere else than an interrupt
     * handler, you need to disable interrupts for synchronization.
     */
    void increment_seconds(uint32_t amount = 1);

    /*
     * This adds i (or 1) seconds to the time.
     */
    Time& operator+(uint32_t i);
    Time& operator++();      // prefix
    Time  operator++(int);   // postfix

    /*
     * Returns the string representation of a weekday.
     * weekday must be in [1, 7].
     */
    static const char *get_weekday_string(uint16_t weekday);

    /*
     * Returns the string representation of a month.
     * month must be in [1, 12].
     */
    static const char *get_month_string(uint16_t month);

    /*
     * Returns the days in a given month. This is necessary due to leap years.
     * month must be in [1, 12].
     * year and century can (theoretically) be anything.
     */
    static uint16_t get_days_per_month(uint16_t month, uint16_t year, uint16_t century);
};
