#pragma once

#include "user/time/cmos.h"
#include "user/time/time.h"
#include "guard/gate.h"
#include "types.h"

class RTC : public CMOS, public Gate, public Time {
private:
	// Disallow copies and assignments.
	RTC(const RTC&)            = delete;
	RTC& operator=(const RTC&) = delete;

    int32_t hz;

public:
    RTC(int16_t timezone = 2) : Time(timezone), hz(-1) {}

    int32_t get_freq() const;

	void init_RTC(bool enable_update_irq = true, CMOS_irq_freq periodic_irq_freq = freq_0hz);

    bool prologue() override;
    void epilogue() override;

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
     * "get_second" etc. are for more convenient use, but internally use "get_value".
     */
    uint16_t get_value(CMOS_offset offset);
    uint16_t get_second();
    uint16_t get_minute();
    uint16_t get_hour();
    uint16_t get_day();
    uint16_t get_month();
    uint16_t get_year();
    uint16_t get_weekday();
    uint16_t get_century();
    uint16_t get_real_year();

    /*
     * "update_time" should be called after using any of these methods.
     */
    void set_value(CMOS_offset offset, uint16_t value);
    void set_second(uint16_t value);
    void set_minute(uint16_t value);
    void set_hour(uint16_t value);
    void set_day(uint16_t value);
    void set_month(uint16_t value);
    void set_year(uint16_t value);
    void set_weekday(uint16_t value);
    void set_century(uint16_t value);
    void set_real_year(uint16_t year);

    /*
     * Sets the member "time" to the current time by reading from the CMOS.
     */
    void update_time();
};

extern RTC rtc;
