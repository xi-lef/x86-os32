#pragma once

#include "types.h"

class Time {
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
    /*
     * These values are uint16_t instead of uint8_t because a uint8_t would be
     * printed as a character, and thus requires a cast. Casts are annoying,
     * so we rather waste some bytes.
     */
    uint16_t second;
    uint16_t minute;
    uint16_t hour;
    uint16_t day;
    uint16_t month;
    uint16_t year;
    uint16_t weekday;
    uint16_t century;

    int16_t timezone;

    Time(int16_t timezone = 0) : timezone(timezone) {}

    void set_timezone(int16_t zone);

    /*
     * Applies the value of "timezone", i.e. "hour" is changed appropriately.
     */
    void apply_timezone();

// Converts hours to seconds. Useful for e.g. increment_seconds.
#define HOURS_TO_SECONDS(x) ((x) * 60 * 60)

    /*
     * Increments/decrements the seconds by amount.
     */
    void increment_seconds(uint32_t amount = 1);
    void decrement_seconds(int32_t amount = 1);

    /*
     * This adds/subtracts i (or 1) seconds to the time.
     */
    Time& operator +=(unsigned int i);
    Time& operator ++();      // Prefix
    Time  operator ++(int);   // Postfix

    Time& operator -=(int i);
    Time& operator --();      // Prefix
    Time  operator --(int);   // Postfix


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
