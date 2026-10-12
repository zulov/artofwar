#pragma once

#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>

#include "database/db_columns.h"

struct SQLConsts {
	template <typename Col>
	static constexpr std::string_view columnType(Col column) {
		if constexpr (std::is_same_v<Col, AimPathCol>) {
			if (column == AimPathCol::path || column == AimPathCol::pending_path) { return "TEXT"; }
		} else if constexpr (std::is_same_v<Col, WorldAgeStateCol>) {
			if (column == WorldAgeStateCol::history) { return "TEXT"; }
		}
		return "INT";
	}

	template <typename Col>
	static std::string makeSchema(std::initializer_list<Col> primaryKey, bool withoutRowid) {
		std::string schema = "(";
		bool firstColumn = true;
		for (const auto column : magic_enum::enum_values<Col>()) {
			if (!firstColumn) { schema += ", "; }
			firstColumn = false;
			schema.append(magic_enum::enum_name(column));
			schema += " ";
			schema.append(columnType(column));
			schema += " NOT NULL";
		}
		if (!primaryKey.empty()) {
			schema += ", PRIMARY KEY(";
			bool firstKey = true;
			for (const auto column : primaryKey) {
				if (!firstKey) { schema += ", "; }
				firstKey = false;
				schema.append(magic_enum::enum_name(column));
			}
			schema += ")";
		}
		schema += ")";
		if (withoutRowid) { schema += " WITHOUT ROWID"; }
		schema += ";";
		return schema;
	}

	inline static const std::string CREATE_TABLE = "CREATE TABLE ";
	inline static const std::string COUNT = "SELECT count(*) from ";
};
