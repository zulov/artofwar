#pragma once

#include <vector>

#include "database/db_world_age_struct.h"
#include "scene/load/RuntimeSaveData.h"

struct db_map;
struct db_world_age;
struct db_world_age_catalog;
struct dbload_container;
class Player;

struct WorldAgePlayerContribution {
	const Player* player{};
	float value{};
	float share{};
};

struct WorldAgeConditionProgress {
	WorldAgeMetric metric{};
	float target{};
	float average{};
	float progress{};
	std::vector<WorldAgePlayerContribution> contributions;
};

struct WorldAgeProgress {
	unsigned short ageId{};
	float progress{};
	std::vector<WorldAgeConditionProgress> conditions;
};

class WorldAgeController {
public:
	static constexpr unsigned AGE_TIMEOUT_TICKS = 18'000;

	WorldAgeController(const db_world_age_catalog* catalog, const db_map* map);

	void reset(unsigned ageStartedTick = 0);
	bool restore(const dbload_container& data);
	void update(const std::vector<Player*>& players, unsigned totalTicks);

	unsigned short getCurrentAgeId() const { return currentAgeId; }
	const db_world_age* getAge(unsigned short ageId) const;
	std::vector<WorldAgeProgress> getNextAgeProgress(const std::vector<Player*>& players) const;
	float getTimeoutProgress(unsigned totalTicks) const;
	bool hasReachedAge(unsigned short ageId) const;
	bool isLevelAvailable(unsigned char ageStage) const;

	WorldAgeStateSaveData saveState() const;
	const std::vector<unsigned short>& getHistory() const { return reachedAgeIds; }
	unsigned getAgeStartedTick() const { return ageStartedTick; }

private:
	float getMetric(const Player& player, const struct db_world_age_condition& condition) const;
	bool ageMet(const std::vector<Player*>& players, const struct db_world_age& age) const;
	float ageProgress(const std::vector<Player*>& players, const struct db_world_age& age) const;
	std::vector<WorldAgeConditionProgress> getConditionProgress(const std::vector<Player*>& players,
		const struct db_world_age& age) const;
	bool isNextAge(const struct db_world_age& current, unsigned short ageId) const;
	unsigned short selectTimeoutAge(const std::vector<Player*>& players, const struct db_world_age& current) const;
	void advance(unsigned short ageId, unsigned totalTicks);

	const db_world_age_catalog* catalog;
	const std::vector<unsigned short>* ageIds;
	unsigned short currentAgeId{};
	unsigned ageStartedTick{};
	std::vector<unsigned short> reachedAgeIds;
};
