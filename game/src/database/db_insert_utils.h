#pragma once
#include <functional>
#include <unordered_map>
#include <sqlite3/sqlite3.h>

#include "db_utils.h"

static void executeBatch(sqlite3* db, const char* sql, const std::function<void(sqlite3_stmt*)>& binderLoop) {
	sqlite3_stmt* stmt = nullptr;

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	ifError(rc, nullptr, sql);

	binderLoop(stmt);

	sqlite3_finalize(stmt);
}

static void stepAndReset(sqlite3_stmt* stmt, const char* sql) {
	int rc = sqlite3_step(stmt);
	ifError(rc, nullptr, sql);

	sqlite3_reset(stmt);
	sqlite3_clear_bindings(stmt);
}


class ParamMap {
public:
	std::unordered_map<std::string, int> map;

	ParamMap(sqlite3_stmt* stmt, const std::vector<std::string>& names) {
		for (auto name : names) { map[name] = sqlite3_bind_parameter_index(stmt, name.c_str()); }
	}

	int operator[](const std::string& key) const {
		const auto it = map.find(key);
		return it != map.end() ? it->second : -1;
	}
};


inline void bindU(sqlite3_stmt* stmt, int idx, unsigned value) {
	sqlite3_bind_int64(stmt, idx, static_cast<sqlite3_int64>(value));
}

inline void bindI(sqlite3_stmt* stmt, int idx, int value) { sqlite3_bind_int(stmt, idx, value); }
inline void bindC(sqlite3_stmt* stmt, int idx, char value) { sqlite3_bind_int(stmt, idx, value); }
inline void bindUC(sqlite3_stmt* stmt, int idx, unsigned char value) { sqlite3_bind_int(stmt, idx, value); }
inline void bindB(sqlite3_stmt* stmt, int idx, bool value) { sqlite3_bind_int(stmt, idx, value); }
inline void bindF(sqlite3_stmt* stmt, int idx, float value) { sqlite3_bind_double(stmt, idx, value); }

inline void bindT(sqlite3_stmt* stmt, int idx, const std::string& value) {
	sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT);
}

template <typename T>
void bindRow(sqlite3_stmt* stmt, const ParamMap& p, int precision, const T* x);

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

inline std::vector<std::string> prefix_with_colon(const std::vector<std::string>& cols) {
	std::vector<std::string> result;
	result.reserve(cols.size());

	for (const auto& c : cols) { result.push_back(":" + c); }

	return result;
}
