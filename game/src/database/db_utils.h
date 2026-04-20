#pragma once
#include <sqlite3/sqlite3.h>
#include <cstdio>
#include <iostream>
#include <type_traits>

inline sqlite3* openDb(const std::string& name, bool readOnly = true) {
	sqlite3* database;
	int flags = (readOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE) | SQLITE_OPEN_NOMUTEX;
	if (const int rc = sqlite3_open_v2(name.c_str(), &database, flags, nullptr)) {
		std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(database) << name << std::endl;
		sqlite3_close_v2(database);
		return nullptr;
	}
	sqlite3_busy_timeout(database, 100);
	return database;
}

inline void ifError(int rc, char* error, const std::string& sql) {
	if (rc != SQLITE_OK && rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error %d: %s\t", rc, error);
		fprintf(stderr, sql.c_str());
		fprintf(stderr, "\n");
		sqlite3_free(error);
	}
}

template <typename Creator>
void loadFromTable(sqlite3* database, const std::string& sqlStr, Creator createFn) {
	const char* sql = sqlStr.c_str();
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "[SQLite ERROR] sqlite3_prepare_v2 failed\n";
		std::cerr << "Code: " << rc << "\n";
		std::cerr << "Message: " << sqlite3_errmsg(database) << "\n";
		std::cerr << "SQL: " << sqlStr << "\n";
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) { createFn(stmt); }

	sqlite3_finalize(stmt);
}

template <typename T>
constexpr int col(T iCol) {
	if constexpr (std::is_enum_v<T>) { return static_cast<int>(iCol); }
	else { return iCol; }
}

template <typename T> inline bool asBool(sqlite3_stmt* stmt, T iCol) { return sqlite3_column_int(stmt, col(iCol)) != 0; }
template <typename T> inline float asFloat(sqlite3_stmt* stmt, T iCol) { return static_cast<float>(sqlite3_column_double(stmt, col(iCol))); }
template <typename T> inline int asInt(sqlite3_stmt* stmt, T iCol) { return sqlite3_column_int(stmt, col(iCol)); }
template <typename T> inline float asItoF(sqlite3_stmt* stmt, T iCol, int precision) { return static_cast<float>(asInt(stmt, iCol)) / precision; }
template <typename T> inline unsigned asUI(sqlite3_stmt* stmt, T iCol) { return static_cast<unsigned>(sqlite3_column_int64(stmt, col(iCol))); }
template <typename T> inline short asShort(sqlite3_stmt* stmt, T iCol) { return static_cast<int16_t>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline unsigned short asUS(sqlite3_stmt* stmt, T iCol) { return static_cast<uint16_t>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline char asByte(sqlite3_stmt* stmt, T iCol) { return static_cast<int8_t>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline unsigned char asUByte(sqlite3_stmt* stmt, T iCol) { return static_cast<unsigned char>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline const char* asText(sqlite3_stmt* stmt, T iCol) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, col(iCol))); }

template <typename T> inline unsigned asHex(sqlite3_stmt* stmt, T iCol) { return static_cast<unsigned>(std::strtoul(asText(stmt, col(iCol)), nullptr, 16)); }

