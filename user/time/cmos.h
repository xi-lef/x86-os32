#pragma once

#include "types.h"
#include "utils/math.h"

class CMOS {
    // Disallow copies and assignments.
    CMOS(const CMOS&)            = delete;
    CMOS& operator=(const CMOS&) = delete;

public:
    CMOS() {}

    const unsigned int nmi_bitmask = 1 << 7;

    // Used to select a value via the control-IO-port.
    enum Offset {
        second  = 0x00,
        minute  = 0x02,
        hour    = 0x04,
        weekday = 0x06,
        day     = 0x07,
        month   = 0x08,
        year    = 0x09,
        statusA = 0x0a,
        statusB = 0x0b,
        statusC = 0x0c,
        statusD = 0x0d,
        century = 0x32,
        alarm_second = 0x01,
        alarm_minute = 0x03,
        alarm_hour   = 0x05
    };

    // Used for manipulating status-register A.
    enum IRQ_freq {
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

    // Used for status-register B and C.
    enum IRQ_enable_bits {
        irq_update   = 1 << 4,
        irq_alarm    = 1 << 5,
        irq_periodic = 1 << 6
    };

    union Status_A {
        struct {
            IRQ_freq freq    : 4;
            uint8_t base     : 3, // Time-base, default: 2 (32768 Hz)
                    updating : 1; // 1 if update is in progess
        };
        uint8_t value;
    };

    union Status_B {
        struct {
            uint8_t summer_time  : 1, // 1 for daylight savings time
                    format_24h   : 1, // 12h (0) or 24h (1)
                    data_mode    : 1, // BCD (0) or binary (1)
                    square_freq  : 1, // weird
                    update_irq   : 1, // update IRQ enabled
                    alarm_irq    : 1, // alarm IRQ (when time == alarm-time) enabled
                    periodic_irq : 1, // periodic IRQ (with frequency from
                                      // status-register A) enabled
                    abort_update : 1; // 1 to abort update in progress
        };
        uint8_t value;
    };

    // Information about the current IRQ (RO).
    // This register needs to be read in order to keep interrupts coming.
    union Status_C {
        struct {
            uint8_t reserved     : 4,
                    update_irq   : 1,
                    alarm_irq    : 1,
                    periodic_irq : 1,
                    irq          : 1; // 1 if there is an IRQ
        };
        uint8_t value;
    };

    int calc_freq(IRQ_freq freq) {
        // Frequency is 65536 / 2^IRQ_freq::freq_xhz.
        return (freq == freq_0hz) ? 0 : (65536 / Math::pow(2, freq));
    }

    /*
     * Initialize the CMOS with fitting values. This includes:
     * * if enable_update_irq is true, enable the update interrupt.
     * * set frequency of periodic interrupt to the frequency corresponding
     *   to periodic_irq_freq.
     *
     * Return the amount of interrupts that will be generated per second.
     */
    uint32_t init(bool enable_update_irq = false, IRQ_freq periodic_irq_freq = freq_0hz);

    /*
     * Read/write the byte from/to the field specified in offset.
     * You must not use the ports directly. Instead, use these methods.
     *
     * "write_port" returns the value from the field specified in offset prior
     * to writing.
     * "select_offset" is just a helper method.
     */
private:
    void select_offset(Offset offset);
public:
    uint8_t read_port(Offset offset);
    uint8_t write_port(Offset offset, uint8_t value);

    /*
     * Methods for getting information about an IRQ.
     * One of these functions MUST be called in order to keep interrupts coming.
     * "clear_statusC" only reads the register but does nothing with its content.
     * The other functions check the respective bits and return the appropriate
     * information.
     */
    void clear_status_c();
    bool is_update_irq();
    bool is_alarm_irq();
    bool is_periodic_irq();
};
