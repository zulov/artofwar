#pragma once

#include <algorithm>
#include <vector>

#include "db_basic_struct.h"
#include "db_columns.h"
#include "db_utils.h"
#include "utils/DeleteUtils.h"

enum class WorldAgeMetric : unsigned char {
	WORKER_COUNT,
	ARMY_COUNT
};

struct db_world_age_condition : db_entity {
	const WorldAgeMetric metric;
	const float target;

	using C = DbWorldAgeConditionCol;
	explicit db_world_age_condition(sqlite3_stmt* stmt)
		: db_entity(asUShort(stmt, C::id)), metric(static_cast<WorldAgeMetric>(asUByte(stmt, C::metric))),
		  target(asFloat(stmt, C::target)) {}
};

struct db_world_age : db_with_name {
	const unsigned char stage;
	std::vector<const db_world_age_condition*> conditions;
	std::vector<unsigned short> nextAgeIds;

	using C = DbWorldAgeCol;
	explicit db_world_age(sqlite3_stmt* stmt)
		: db_with_name(asUShort(stmt, C::id), asText(stmt, C::name)), stage(asUByte(stmt, C::stage)) {}
};

struct db_world_age_catalog {
	std::vector<db_world_age*> ages;
	std::vector<db_world_age_condition*> conditions;

	~db_world_age_catalog() {
		clear_vector(ages);
		clear_vector(conditions);
	}

	const db_world_age* getAge(unsigned short id) const {
		return id < ages.size() ? ages[id] : nullptr;
	}

	db_world_age* getAge(unsigned short id) { return id < ages.size() ? ages[id] : nullptr; }

	const db_world_age_condition* getCondition(unsigned short id) const {
		return id < conditions.size() ? conditions[id] : nullptr;
	}

	unsigned char maxStage() const {
		unsigned char result = 0;
		for (const auto* age : ages) {
			if (age) {
				result = std::max(result, age->stage);
			}
		}
		return result;
	}
};
