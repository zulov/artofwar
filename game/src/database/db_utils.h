#pragma once
#include <sqlite3/sqlite3.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <type_traits>

inline sqlite3* openDb(const std::string& name, bool readOnly = true, std::string* error = nullptr) {
	sqlite3* database = nullptr;
	int flags = (readOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE) | SQLITE_OPEN_NOMUTEX;
	if (const int rc = sqlite3_open_v2(name.c_str(), &database, flags, nullptr)) {
		const std::string detail = database ? sqlite3_errmsg(database) : "unknown error";
		if (error) {
			*error = "open failed (code " + std::to_string(rc) + "): " + detail;
		}
		std::cerr << "Error opening SQLite3 database: " << detail << " " << name << std::endl;
		if (database) {
			sqlite3_close_v2(database);
		}
		return nullptr;
	}
	sqlite3_busy_timeout(database, 100);
	return database;
}

inline bool ifError(int rc, const char* error, const std::string& sql) {
	if (rc == SQLITE_OK || rc == SQLITE_DONE) {
		return true;
	}
	fprintf(stderr, "SQL error %d: %s\t%s\n", rc, error ? error : "(no details)", sql.c_str());
	return false;
}

inline bool execSql(sqlite3* database, const char* sql) {
	char* error = nullptr;
	const int rc = sqlite3_exec(database, sql, nullptr, nullptr, &error);
	const bool success = ifError(rc, error, sql);
	if (error) {
		sqlite3_free(error);
	}
	return success;
}

template <typename Creator>
bool loadFromTable(sqlite3* database, const std::string& sqlStr, Creator createFn, std::string* error = nullptr) {
	auto reportError = [&](const std::string& message) {
		if (error) {
			*error = message;
		}
		std::cerr << "[SQLite ERROR] " << message << "\n";
	};

	if (!database) {
		reportError("database is not open");
		return false;
	}
	const char* sql = sqlStr.c_str();
	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		reportError("prepare failed (code " + std::to_string(rc) + "): " + sqlite3_errmsg(database) +
				"; SQL: " + sqlStr + "; likely an old save schema or missing column");
		return false;
	}

	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { createFn(stmt); }
	if (rc != SQLITE_DONE) {
		reportError("query failed (code " + std::to_string(rc) + "): " + sqlite3_errmsg(database) +
				"; SQL: " + sqlStr);
	}

	sqlite3_finalize(stmt);
	return rc == SQLITE_DONE;
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
template <typename T> inline float asScaledFloat(sqlite3_stmt* stmt, T iCol, int precision) {
	return sqlite3_column_type(stmt, col(iCol)) == SQLITE_FLOAT ? asFloat(stmt, iCol) : asItoF(stmt, iCol, precision);
}
template <typename T> inline unsigned asUI(sqlite3_stmt* stmt, T iCol) { return static_cast<unsigned>(sqlite3_column_int64(stmt, col(iCol))); }
template <typename T> inline short asShort(sqlite3_stmt* stmt, T iCol) { return static_cast<int16_t>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline unsigned short asUShort(sqlite3_stmt* stmt, T iCol) { return static_cast<uint16_t>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline char asByte(sqlite3_stmt* stmt, T iCol) { return static_cast<int8_t>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline unsigned char asUByte(sqlite3_stmt* stmt, T iCol) { return static_cast<unsigned char>(sqlite3_column_int(stmt, col(iCol))); }
template <typename T> inline const char* asText(sqlite3_stmt* stmt, T iCol) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, col(iCol))); }

template <typename T> inline unsigned asHex(sqlite3_stmt* stmt, T iCol) { return static_cast<unsigned>(std::strtoul(asText(stmt, col(iCol)), nullptr, 16)); }

template <typename T>
T readRow(sqlite3_stmt* stmt, int precision);

