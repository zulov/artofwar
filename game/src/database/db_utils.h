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

inline bool asBool(sqlite3_stmt* stmt, int iCol) { return sqlite3_column_int(stmt, iCol) != 0; }
inline float asFloat(sqlite3_stmt* stmt, int iCol) { return static_cast<float>(sqlite3_column_double(stmt, iCol)); }
inline int asInt(sqlite3_stmt* stmt, int iCol) { return sqlite3_column_int(stmt, iCol); }
inline short asShort(sqlite3_stmt* stmt, int iCol) { return static_cast<int16_t>(sqlite3_column_int(stmt, iCol)); }
inline const char* asText(sqlite3_stmt* stmt, int iCol) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, iCol)); }