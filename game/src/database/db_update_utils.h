#pragma once

#include <sqlite3/sqlite3.h>
#include <Urho3D/Container/Str.h>

struct DbUpdate {
	sqlite3_stmt* stmt = nullptr;

	DbUpdate(sqlite3* db, const char* sql) { sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr); }

	~DbUpdate() {
		if (stmt)
			sqlite3_finalize(stmt);
	}

	void reset() {
		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
	}

	int param(const char* name) { return sqlite3_bind_parameter_index(stmt, name); }

	// bind by name
	DbUpdate& bind(const char* name, int v) {
		sqlite3_bind_int(stmt, param(name), v);
		return *this;
	}

	DbUpdate& bind(const char* name, bool v) {
		sqlite3_bind_int(stmt, param(name), v ? 1 : 0);
		return *this;
	}

	DbUpdate& bind(const char* name, float v) {
		sqlite3_bind_double(stmt, param(name), static_cast<double>(v));
		return *this;
	}

	DbUpdate& bind(const char* name, const Urho3D::String& v) {
		sqlite3_bind_text(stmt, param(name), v.CString(), -1, SQLITE_TRANSIENT);
		return *this;
	}

	bool exec() { return sqlite3_step(stmt) == SQLITE_DONE; }
	bool execAndClose() {
		auto result = exec();
		if (stmt)
			sqlite3_finalize(stmt);
		return result;
	}

};
