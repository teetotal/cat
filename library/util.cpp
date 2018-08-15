﻿#include "pch.h"
#include <stdio.h>
#include <stdlib.h>

time_t getNow() {
	return time(0);
}

void sleepThisThread(size_t msec) {
	std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

int getRandValue(int max) {
	if (max == 0)
		return 0;
	return rand() % max;
}

int rounding(float f) {
	return (int)round(f);
}

string loadJsonString(const char *path) {
	ifstream fileopen;
	fileopen.open(path, ios::in | ios::binary);
	string str((std::istreambuf_iterator<char>(fileopen)), std::istreambuf_iterator<char>());
	fileopen.close();

	return str;
}

void saveFile(const char* path, string sz) {
#ifdef WIN32
	std::ofstream out(path);
	out << sz;
	out.close();
#endif
}

time_t getTime(int hour, int min, int sec) {
	time_t now = time(0);
	tm *ltm = localtime(&now);
	ltm->tm_hour = hour;
	ltm->tm_min = min;
	ltm->tm_sec = sec;

	return mktime(ltm);

}
tm * getLocalTm(time_t t) {
	return localtime(&t);
}
string wstring_to_utf8(const wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

wstring utf8_to_utf16(const string& str){
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}
/*
char* intToChar(int n) {
	char *buf = new char[10];
	return ::itoa(n, buf, 10);
	
}
 */
/*
wstring utf8_to_utf16(const string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		//bool error = false;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni = ch;
			todo = 0;
		}
		else if (ch <= 0xBF)
		{
			throw std::logic_error("not a UTF-8 string");
		}
		else if (ch <= 0xDF)
		{
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
			todo = 3;
		}
		else
		{
			throw std::logic_error("not a UTF-8 string");
		}
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size())
				throw std::logic_error("not a UTF-8 string");
			unsigned char ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF)
				throw std::logic_error("not a UTF-8 string");
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF)
			throw std::logic_error("not a UTF-8 string");
		if (uni > 0x10FFFF)
			throw std::logic_error("not a UTF-8 string");
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (size_t i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF)
		{
			utf16 += (wchar_t)uni;
		}
		else
		{
			uni -= 0x10000;
			utf16 += (wchar_t)((uni >> 10) + 0xD800);
			utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
		}
	}
	return utf16;
}
*/