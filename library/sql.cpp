#include "sql.h"

Sql * Sql::hInst = NULL;

bool Sql::init(string path) {	
	int rc = sqlite3_open(path.c_str(), &mDB);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(mDB));
		finalize();
		return false;
	}
	
	return true;
}
/*
void Sql::finalize() {
	sqlite3_close(mDB);
}
*/
int Sql::exec(string query) {
	char *zErrMsg = 0;
	int rc = sqlite3_exec(mDB, query.c_str(), NULL, NULL, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);		
	}	
	return rc;
}

sqlite3_stmt * Sql::select(string query) {
	sqlite3_stmt* statement;

	if (sqlite3_prepare_v2(mDB, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		return statement;
		/*
		int result = 0;

		while (true)
		{
			result = sqlite3_step(statement);

			if (result == SQLITE_ROW)
			{
				// do something with the row.
				
			}
			else
			{
				return false;
			}
		}
		
		return true;
		*/
	}
	return NULL;
}