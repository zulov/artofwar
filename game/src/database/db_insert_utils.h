#pragma once
#include <functional>
#include <sqlite3/sqlite3.h>

#include "db_utils.h"

inline void executeBatch(sqlite3* db, const char* sql, const std::function<void(sqlite3_stmt*)>& binderLoop) {
	sqlite3_stmt* stmt = nullptr;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	ifError(rc, nullptr, sql);

	binderLoop(stmt);

	sqlite3_finalize(stmt);
}

inline void stepAndReset(sqlite3_stmt* stmt, const char* sql) {
	int rc = sqlite3_step(stmt);
	ifError(rc, nullptr, sql);

	sqlite3_reset(stmt);
	sqlite3_clear_bindings(stmt);
}

template <typename E>
constexpr int bindIdx(E e) { return static_cast<int>(e) + 1; }

template <typename E> void bindU(sqlite3_stmt* stmt, E col, unsigned value) {
	sqlite3_bind_int64(stmt, bindIdx(col), static_cast<sqlite3_int64>(value));
}
template <typename E> void bindI(sqlite3_stmt* stmt, E col, int value) { sqlite3_bind_int(stmt, bindIdx(col), value); }
template <typename E> void bindC(sqlite3_stmt* stmt, E col, char value) { sqlite3_bind_int(stmt, bindIdx(col), value); }
template <typename E> void bindUC(sqlite3_stmt* stmt, E col, unsigned char value) { sqlite3_bind_int(stmt, bindIdx(col), value); }
template <typename E> void bindB(sqlite3_stmt* stmt, E col, bool value) { sqlite3_bind_int(stmt, bindIdx(col), value); }
template <typename E> void bindF(sqlite3_stmt* stmt, E col, float value) { sqlite3_bind_double(stmt, bindIdx(col), value); }
template <typename E> void bindT(sqlite3_stmt* stmt, E col, const std::string& value) {
	sqlite3_bind_text(stmt, bindIdx(col), value.c_str(), -1, SQLITE_TRANSIENT);
}

template <typename T>
void bindRow(sqlite3_stmt* stmt, int precision, const T* x);

inline std::string make_insert_sql(const std::string& table, const std::vector<std::string>& cols) {
	std::string sql = "INSERT INTO " + table + " (";

	for (size_t i = 0; i < cols.size(); ++i) {
		sql += cols[i];
		if (i + 1 < cols.size()) { sql += ", "; }
	}

	sql += ") VALUES (";

	for (size_t i = 0; i < cols.size(); ++i) {
		sql += ":" + cols[i];
		if (i + 1 < cols.size()) { sql += ", "; }
	}

	sql += ");";
	return sql;
}
