#pragma once
#include <sqlite3/sqlite3.h>
#include <cstdio>

inline void ifError(int rc, char* error, const std::string& sql) {
	if (rc != SQLITE_OK && rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error %d: %s\t", rc, error);
		fprintf(stderr, sql.c_str());
		fprintf(stderr, "\n");
		sqlite3_free(error);
	}
}
