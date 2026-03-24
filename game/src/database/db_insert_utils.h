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

	ParamMap(sqlite3_stmt* stmt, std::initializer_list<const char*> names) {
		for (auto name : names) {
			map[name] = sqlite3_bind_parameter_index(stmt, name);
		}
	}

	int operator[](const std::string& key) const { return map.at(key); }
};


inline void bindValue(sqlite3_stmt* stmt, int idx, int value) { sqlite3_bind_int(stmt, idx, value); }

inline void bindValue(sqlite3_stmt* stmt, int idx, double value) { sqlite3_bind_double(stmt, idx, value); }

inline void bindValue(sqlite3_stmt* stmt, int idx, const std::string& value) {
	sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT);
}

template <typename T>
void bindRow(sqlite3_stmt* stmt, const ParamMap& p, const T& obj);
