#pragma once
struct intPair {
	int key;
	int val;
};
time_t getNow();
int getRandValue(int max);
void sleepThisThread(size_t msec);
time_t getTime(int hour, int min, int sec);
int rounding(float f); //반올림


