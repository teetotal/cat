
#ifndef PROJ_ANDROID_SQL_H
#define PROJ_ANDROID_SQL_H

#if defined(_WIN32) && !defined(COCOS2D_DEBUG)

#else
	#include "sqlite3/include/sqlite3.h"
#endif

#include "pch.h"

class Sql {
public:
	Sql() {
		if (hInst == NULL)
			hInst = this;
	};
	~Sql() {
		if (hInst != NULL)
			hInst = NULL;
	};
	static Sql * inst() {
		return hInst;
	};

	bool init(string path);
	void finalize() {
		sqlite3_close(mDB);
	};

	int exec(string query);
	sqlite3_stmt * select(string query);

private:
	static Sql * hInst;
	sqlite3 *mDB;
};

#endif 