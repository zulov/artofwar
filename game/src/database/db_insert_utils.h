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


inline void bindU(sqlite3_stmt* stmt, int idx, unsigned value) {
	sqlite3_bind_int64(stmt, idx, static_cast<sqlite3_int64>(value));
}

inline void bindI(sqlite3_stmt* stmt, int idx, int value) { sqlite3_bind_int(stmt, idx, value); }

inline void bindB(sqlite3_stmt* stmt, int idx, char value) { sqlite3_bind_int(stmt, idx, value); }

inline void bindF(sqlite3_stmt* stmt, int idx, float value) { sqlite3_bind_double(stmt, idx, value); }

inline void bindS(sqlite3_stmt* stmt, int idx, const std::string& value) {
	sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT);
}

template <typename T>
void bindRow(sqlite3_stmt* stmt, const ParamMap& p, int precision, const T* x);

#include <array>

template <size_t N, size_t BufSize = 512>
constexpr auto make_insert_sql(const char* table, const std::array<const char*, N>& cols) {
	std::array<char, BufSize> out{};
	size_t pos = 0;

	auto append = [&](const char* s) {
		while (*s)
			out[pos++] = *s++;
	};

	append("INSERT INTO ");
	append(table);
	append(" (");

	// columns
	for (size_t i = 0; i < N; ++i) {
		append(cols[i]);
		if (i + 1 < N)
			append(", ");
	}

	append(") VALUES (");

	// params (:col)
	for (size_t i = 0; i < N; ++i) {
		out[pos++] = ':';
		append(cols[i]);
		if (i + 1 < N)
			append(", ");
	}

	append(");");
	out[pos] = '\0';

	return out;
}

template <size_t N>
constexpr auto prefix_with_colon(const std::array<const char*, N>& cols) {
	std::array<std::array<char, 64>, N> storage{}; // holds ":column"
	std::array<const char*, N> result{};

	for (size_t i = 0; i < N; ++i) {
		auto& buf = storage[i];
		size_t pos = 0;

		buf[pos++] = ':';

		const char* s = cols[i];
		while (*s) {
			buf[pos++] = *s++;
		}

		buf[pos] = '\0';
		result[i] = buf.data();
	}

	return std::pair{storage, result};
}
