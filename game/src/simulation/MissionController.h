#pragma once

#include <vector>
#include <ranges>

#include "database/db_other_struct.h"
#include "scene/load/RuntimeSaveData.h"

class Player;

enum class MissionResult : unsigned char { RUNNING, VICTORY, DEFEAT };

struct MissionGoalState {
	const db_mission_goal* goal{};
	float value{};
	float target{};
	bool complete{};
	bool deadlineEvaluated{};
	bool deadlineSuccess{};
	float deadlineScore{};
	float deadlineEnemyScore{};
	unsigned char deadlinePlayer{};
};

class MissionController {
public:
	explicit MissionController(const db_map* map);

	void update(Player* activePlayer, const std::vector<Player*>& players, unsigned totalTicks);
	const std::vector<MissionGoalState>& getGoals() const { return goals; }
	MissionResult getResult() const { return result; }
	const db_map* getMap() const { return map; }
	std::vector<MissionGoalDeadlineSaveData> saveDeadlines(unsigned char player) const;
	void restoreDeadlines(const std::vector<MissionGoalDeadlineSaveData>& deadlines, unsigned char player);

private:
	bool goalValue(const db_mission_goal& goal, Player& player, const std::vector<Player*>& players,
				unsigned totalTicks, float& value, float& target) const;
	bool hasLivingEntities(const Player& player, bool units, bool buildings) const;
	unsigned enemyEntityCount(const Player& player, const std::vector<Player*>& players, bool units, bool buildings,
						  short targetPlayer, bool targetTeam = false) const;
	Player* getTargetPlayer(short targetPlayer, const Player& player, const std::vector<Player*>& players) const;

	const db_map* map;
	std::vector<MissionGoalState> goals;
	short lastActivePlayer = -1;
	MissionResult result = MissionResult::RUNNING;
};
