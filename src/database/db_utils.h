#pragma once
#include <sqlite3/sqlite3.h>
#include <cstdio>

inline void ifError(int rc, char* error) {
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", error);
		sqlite3_free(error);
	}
}
