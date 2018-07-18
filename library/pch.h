#include <iostream>
#include <time.h>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <cmath>
#include "util.h"

#define SEC 1000

using namespace std;

typedef map<int, int> intMap;
typedef void(*farmingFinshedNotiCallback)(int fieldId);
typedef void(*tradeUpdatedCallback)(time_t t);
wstring utf8_to_utf16(const string& utf8);

