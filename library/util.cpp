#include "pch.h"

time_t getNow() {
	return time(0);
}

void sleepThisThread(size_t msec) {
	std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}