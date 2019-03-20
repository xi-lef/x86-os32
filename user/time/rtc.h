#pragma once

#include "user/time/cmos.h"
#include "user/time/time.h"
#include "guard/gate.h"
#include "types.h"

class RTC : public CMOS, public Gate {
private:
	// Disallow copies and assignments.
	RTC(const RTC&)            = delete;
	RTC& operator=(const RTC&) = delete;

    Time time;
    int32_t hz;
    uint32_t jiffies;

public:
    RTC(int16_t timezone = 1) : time(timezone), hz(-1), jiffies(0) {}

    Time get_time() const {
        return time;
    }

    int32_t get_freq() const {
        return hz;
    }

    uint32_t get_jiffies() const {
        return jiffies;
    }

	void init_RTC(bool enable_update_irq = true, CMOS_irq_freq periodic_irq_freq = freq_0hz);

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
     * get_second etc. are for more convenient use, but internally use get_value.
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
     * Sets time to the current time by reading from the CMOS.
     */
    void set_time();

    /*
     * DO NOT USE THIS. Use [Guarded_]Bell::sleep instead.
     * sleep waits for [t - 1, t] seconds. dont rely on precision!
     */
    void sleep(unsigned int t);
};

extern RTC rtc;
