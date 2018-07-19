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

#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include "util.h"

#define SEC 1000

using namespace std;
using namespace rapidjson;

typedef map<int, int> intMap;
typedef void(*farmingFinshedNotiCallback)(int fieldId);
typedef void(*tradeUpdatedCallback)(time_t t);
typedef void(*achievementCallback)(bool isDaily, int idx);
wstring utf8_to_utf16(const string& utf8);
string loadJsonString(const char *path);

