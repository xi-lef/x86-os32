#pragma once

#include "user/time/cmos.h"
#include "user/time/time.h"
#include "guard/gate.h"
#include "types.h"

class RTC : public CMOS, public Time, public Gate {
private:
	// Disallow copies and assignments.
	RTC(const RTC&)            = delete;
	RTC& operator=(const RTC&) = delete;

public:
    int32_t hz;
	RTC(int8_t timezone = 1) : Time(timezone), hz(-1) {}

	void init_RTC();

    bool prologue();
    void epilogue();

    /*
     * RTC starts weeks at sunday. :(
     */
    enum Weekday {
        sunday = 1,
        monday,
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
    };

    enum Month {
        january = 1,
        february,
        march,
        april,
        may,
        june,
        july,
        august,
        september,
        october,
        november,
        december
    };

    /*
     * These methods read the corresponding values from the CMOS.
     * get_value is just a helper method.
     */
    uint16_t get_value(CMOS_offset offset);

    uint16_t get_second ();
    uint16_t get_minute ();
    uint16_t get_hour   ();
    uint16_t get_day    ();
    uint16_t get_month  ();
    uint16_t get_year   ();
    uint16_t get_weekday();
    uint16_t get_century();

    /*
     * Sets the values in superclass Time by reading from the CMOS.
     */
    void set_time();

    /* TODO improve, static?
     * sleep waits for [time - 1, time] seconds. dont rely on precision!
     * sleep(1) can only really be used to wait until the next second starts
     * (good for clock displays ;)).
     */
    void sleep(unsigned int time);
};

extern RTC rtc;
