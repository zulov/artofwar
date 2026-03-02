#pragma once
#include <charconv>
#include <sqlite3/sqlite3.h>
#include <cstdio>
#include <ios>
#include <sstream>

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
inline unsigned short asUS(sqlite3_stmt* stmt, int iCol) { return static_cast<uint16_t>(sqlite3_column_int(stmt, iCol)); }
inline char asByte(sqlite3_stmt* stmt, int iCol) { return static_cast<int8_t>(sqlite3_column_int(stmt, iCol)); }
inline const char* asText(sqlite3_stmt* stmt, int iCol) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, iCol)); }

inline const unsigned asHex(sqlite3_stmt* stmt, int iCol) {
	return static_cast<unsigned>(std::strtoul(asText(stmt, iCol), nullptr, 16));
}

static int callback(void* data, int argc, char** argv, char** azColName) {
	return 0;
}