#pragma once
#include <sqlite3/sqlite3.h>
#include <cstdio>

inline void ifError(int rc, char* error) {
	if (rc != SQLITE_OK && rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error %d: %s\n", rc, error);
		sqlite3_free(error);
	}
}
