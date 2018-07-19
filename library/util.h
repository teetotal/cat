#pragma once
struct intPair {
	int key;
	int val;
};
time_t getNow();
int getRandValue(int max);
void sleepThisThread(size_t msec);
int rounding(float f); //반올림

