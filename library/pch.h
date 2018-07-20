#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <cmath>
#include <codecvt>
#include <cwchar>



#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/writer.h"
#include "util.h"

#define SEC 1000
#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace rapidjson;

typedef map<int, int> intMap;
typedef void(*farmingFinshedNotiCallback)(int fieldId);
typedef void(*tradeUpdatedCallback)(time_t t);
typedef void(*achievementCallback)(bool isDaily, int idx);
wstring utf8_to_utf16(const string& utf8);
string wstring_to_utf8(const wstring& str);
string loadJsonString(const char *path);


