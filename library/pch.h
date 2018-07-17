#include <iostream>
#include <time.h>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include "util.h"

using namespace std;

typedef map<int, int> intMap;
typedef void(*farmingFinshedNotiCallback)(int fieldId);
wstring utf8_to_utf16(const string& utf8);
