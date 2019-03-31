#pragma once

#include "machine/io_port.h"
#include "types.h"
#include "utils/math.h"

class CMOS {
private:
    // Disallow copies and assignments.
    CMOS(const CMOS&)            = delete;
    CMOS& operator=(const CMOS&) = delete;

public:
    const IO_Port CMOS_ctrl_port;
    const IO_Port CMOS_data_port;

    CMOS() : CMOS_ctrl_port(0x70), CMOS_data_port(0x71) {}

    // Used for CMOS_ctrl_port.
    enum CMOS_offset {
        offset_second  = 0x00,
        offset_minute  = 0x02,
        offset_hour    = 0x04,
        offset_weekday = 0x06,
        offset_day     = 0x07,
        offset_month   = 0x08,
        offset_year    = 0x09,
        offset_statusA = 0x0a,
        offset_statusB = 0x0b,
        offset_statusC = 0x0c,
        offste_statusD = 0x0d,
        offset_century = 0x32
    };

    // Used for manipulating status-register B.
    enum CMOS_irq_enable_bits {
        irq_update   = 1 << 4,
        irq_alarm    = 1 << 5,
        irq_periodic = 1 << 6
    };

//Frequency is 2^16 / 2^CMOS_irq_freq::freq_xhz.
#define CMOS_CALC_FREQ(freq) ((freq == freq_0hz) ? 0 : (Math::pow(2, 16) / Math::pow(2, freq)))

    // Used for manipulating status-register A.
    enum CMOS_irq_freq {
        freq_0hz    = 0,
        freq_2hz    = 15,
        freq_4hz    = 14,
        freq_8hz    = 13,
        freq_16hz   = 12,
        freq_32hz   = 11,
        freq_64hz   = 10,
        freq_128hz  = 9,
        freq_256hz  = 8,
        freq_512hz  = 7,
        freq_1024hz = 6,
        freq_2048hz = 5,
        freq_4096hz = 4,
        freq_8192hz = 3,
    };

    /*
     * Initializes the CMOS with fitting values. This includes:
     * * if enable_update_irq is true, enable the update interrupt.
     * * set frequency of periodic interrupt to the frequency corresponding
     *   to periodic_irq_freq.
     *
     * Returns the amount of interrupts that will be generated per second.
     */
    uint32_t init_CMOS(bool enable_update_irq = false, CMOS_irq_freq periodic_irq_freq = freq_0hz);

    /*
     * Reads/writes the byte from/to the field specified in offset.
     * You must not use the ports directly. Instead, use these methods.
     *
     * "write_port" returns the value from the field specified in offset prior
     * to writing.
     * "select_offset" is just a helper method.
     */
    void select_offset(CMOS_offset offset);
    uint8_t read_port(CMOS_offset offset);
    uint8_t write_port(CMOS_offset offset, uint8_t value);

    /*
     * Helper methods for getting information about an IRQ.
     * One of these functions MUST be called in order to keep interrupts coming.
     * "clear_statusC" only reads the register but does nothing with its content.
     * The other functions check the respective bits and return the appropriate
     * information.
     */
    void clear_statusC();
    bool is_update_irq();
    bool is_alarm_irq();
    bool is_periodic_irq();

    /*
     * Binary Coded Decimal to Integer conversion (and vice versa).
     * Assumes an 8 bit BCD, so only values from 0 to 99 are supported.
     */
    uint16_t bcd_to_int(uint8_t bcd);
    uint16_t int_to_bcd(uint8_t i);
};
