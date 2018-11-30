#pragma once

#include "guard/gate.h"

class RTC : public Gate {
private:
	// Verhindere Kopien und Zuweisungen
	RTC(const RTC&)            = delete;
	RTC& operator=(const RTC&) = delete;

public:

	RTC() {}

	void init();

	void trigger();
};

extern RTC rtc;
