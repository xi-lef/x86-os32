// vim: set et ts=4 sw=4:

#include "machine/lapic.h"
#include "machine/io_port.h"
#include "utils/math.h"

// global object definition
LAPIC lapic;

uint32_t LAPIC::LAPIC_BASE = 0xfee00000;

void LAPIC::write(uint16_t reg, LAPICRegister_t val)
{
	*((volatile uint32_t *) (LAPIC_BASE + reg)) = val.value;
}

LAPICRegister_t LAPIC::read(uint16_t reg)
{
	LAPICRegister_t value;
	value.value = *((volatile uint32_t *) (LAPIC_BASE + reg));
	return value;
}

uint8_t LAPIC::getLAPICID()
{
	LAPICRegister_t value = read(lapicid_reg);
	if (isPentium4orNewer()) {
		// Pentium 4 or Xeon
		return value.lapicid_p4.apic_id;
	} else {
		// Pentium
		return value.lapicid_p.apic_id;
	}
}

uint8_t LAPIC::getVersion()
{
	LAPICRegister_t value = read(lapicver_reg);
	return value.lapicver.version;
}

void LAPIC::init()
{
	LAPICRegister_t value;

	// reset logical destination ID
	// can be set using setLogicalLAPICID()
	value = read(ldr_reg);
	value.ldr.lapic_id = 0;
	write(ldr_reg, value);

	// set task priority to 0 -> accept all interrupts
	value = read(tpr_reg);
	value.tpr.task_prio = 0;
	value.tpr.task_prio_sub = 0;
	write(tpr_reg, value);

	// set flat delivery mode
	value = read(dfr_reg);
	value.dfr.model = DESTINATION_MODEL_FLAT;
	write(dfr_reg, value);

	// use 255 as spurious vector, enable APIC and disable focus processor
	value = read(spiv_reg);
	value.svr.spurious_vector = 0xff;
	value.svr.apic_enable = LAPIC_ENABLED;
	value.svr.focus_processor_checking = FOCUS_CPU_DISABLED;
	write(spiv_reg, value);
}

void LAPIC::ackIRQ()
{
	// dummy read
	read(spiv_reg);

	// signal end of interrupt
	LAPICRegister_t eoi;
	eoi.value = 0;
	write(eoi_reg, eoi);
}

void LAPIC::setLogicalLAPICID(uint8_t id)
{
	LAPICRegister_t value = read(ldr_reg);
	value.ldr.lapic_id = id;
	write(ldr_reg, value);
}

void LAPIC::sendIPI(uint8_t destination, struct ICR_L data)
{
	// wait for the last IPI to finish
	while(!isIPIDelivered()) {
		// busy
	}

	// set destination in ICR_H register
	LAPICRegister_t value = read(icrh_reg);
	value.icr_h.destination_field = destination;
	write(icrh_reg, value);

	// write data to ICR register
	// write triggers the IPI send
	value.icr_l = data;
	write(icrl_reg, value);
}

bool LAPIC::isIPIDelivered()
{
	LAPICRegister_t value = read(icrl_reg);
	return value.icr_l.delivery_status == DELIVERY_STATUS_IDLE;
}

bool LAPIC::isExternalAPIC()
{
	// System Programming Guide 3A, p. 9-6
	uint8_t version = getVersion();
	if ((version & 0xf0) == 0x00) {
		return true;
	} else {
		return false;
	}
}

bool LAPIC::isLocalAPIC()
{
	// System Programming Guide 3A, p. 9-6
	uint8_t version = getVersion();
	if ((version & 0xf0) == 0x10) {
		return true;
	} else {
		return false;
	}
}

bool LAPIC::isPentium4orNewer()
{
	// System Programming Guide 3A, p. 9-6
	uint8_t version = getVersion();
	if (version >= 0x14) {
		return true;
	} else {
		return false;
	}
}

uint32_t LAPIC::timer_ticks()
{
	// PIT ports
	IO_Port ctrl(0x43);
	IO_Port data(0x42);
	IO_Port help(0x61);

	// start LAPIC timer, single shot, no IRQs
	setTimer(0xffffffff, 1, 42, false, true);

	// wind up PIT to count down from (2^16)-1 (-> takes 54.91833 ms)
	// (speaker disable, timer gate enable -> 00000001)
	help.outb(0x01);
	ctrl.outb(0xB0);
	data.outb(0xFF);
	data.outb(0xFF);

	// read current LAPIC timer counter
	uint32_t ticks = read(ccr_reg).value;

	// wait for PIT to finish
	while(!(help.inb() & 0x20));

	// read current LAPIC timer counter again
	ticks = ticks - read(ccr_reg).value;

	// disable LAPIC timer, single shot, no IRQs
	setTimer(0, 1, 42, false, true);

	uint32_t freq = (uint32_t)Math::div64(((uint64_t) ticks) * 1000 * 1000, 838 * 65535);
	return freq;
}

uint8_t LAPIC::timer_div(uint8_t div)
{
	// LAPIC timer divider table:
	// value    divides by
	// 0xb        1
	// 0x0        2
	// 0x1        4
	// 0x2        8
	// 0x3       16
	// 0x8       32
	// 0x9       64
	// 0xa      128
	const uint8_t masks[] = {0xb, 0x0, 0x1, 0x2, 0x3, 0x8, 0x9, 0xa};

	// is div not a power of two, or zero?
	if(!((div != 0) && !(div & (div - 1)))) {
		return 0xff;
	}

	int trail = __builtin_ctz(div); // count trailing 0-bits
	if (trail > 7) {
		return 0xff;
	}

	return masks[trail];
}

void LAPIC::setTimer(uint32_t counter, uint8_t divide, uint8_t vector, bool periodic, bool masked) {
    // set initial count to 0 to stop timer for configuration
    LAPICRegister_t lr = { .value = 0 };
    write(icr_reg, lr);

    // configure timer control register
    lr = read(timerctrl_reg);
    lr.timer_ctrl.vector     = vector;
    lr.timer_ctrl.masked     = masked;
    lr.timer_ctrl.timer_mode = periodic;
    write(timerctrl_reg, lr);

    // configure divide configuration register
    lr.value = timer_div(divide);
    write(dcr_reg, lr);

    // configure initial count register. this also starts the timer
    lr.value = counter;
    write(icr_reg, lr);
}

void LAPIC::setTimerMasked(bool masked) {
    LAPICRegister_t lr = read(timerctrl_reg);
    lr.timer_ctrl.masked = masked;
    write(timerctrl_reg, lr);
}
