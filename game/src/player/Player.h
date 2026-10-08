#pragma once

#include <vector>
#include "ai/AiHistory.h"
#include "ai/AiOrchestrator.h"
#include "database/db_insert_utils.h"
#include "objects/queue/QueueManager.h"

class Possession;
class Resources;
class Building;
struct db_nation;
struct db_unit;
struct db_unit_level;
struct db_building;
struct db_building_level;
struct db_technology_level;
struct db_with_cost;
enum class ActionType : unsigned char;

template <typename T>
struct PlayerLevel {
	int id = -1;
	unsigned char level = 0;
	T* effective = nullptr;
};

class Player {
	friend void bindRow<Player>(sqlite3_stmt*, int, const Player*);

public:
	Player(unsigned char nationId, unsigned char team, unsigned char id, unsigned char color, Urho3D::String name,
		   bool active, unsigned currentBuildingUId, unsigned currentUnitUId);
	~Player();

	void setResourceAmount(float food, float wood, float stone, float gold) const;
	void setResourceAmount(float amount) const;
	char upgradeLevel(QueueActionType type, int id);
	bool startTechnologyResearch(unsigned short levelId, Building* building);
	unsigned short technologyResearchDuration(unsigned short levelId) const;
	db_with_cost technologyResearchCost(unsigned short levelId) const;

	Resources* getResources() const { return resources; }
	Possession* getPossession() const { return possession; }
	unsigned char getNation() const;
	unsigned char getTeam() const { return team; }
	unsigned char getId() const { return id; } // TODO bug id playera a jego index to cz�sto nie to samo
	unsigned char getColor() const { return color; }
	const Urho3D::String& getName() const { return name; }

	void updateResource1s() const;
	void updateResourceMonth() const;
	void updateResourceYear() const;

	void updatePossession();
	void add(Unit* unit) const;
	void add(Building* building) const;
	void aiAction();
	void aiOrder();
	int getScore();

	int getWorkersNumber() const;

	QueueElement* updateQueue();
	QueueManager& getQueue() { return queue; }
	const QueueManager& getQueue() const { return queue; }
	db_unit_level* getUnitLevel(unsigned short id) const;
	db_building_level* getBuildingLevel(unsigned short id) const;
	bool isBuildingAvailable(unsigned short id) const;
	std::optional<db_unit_level*> getNextUnitLevel(unsigned short id) const;
	std::optional<db_building_level*> getNextBuildingLevel(unsigned short id) const;
	void addKilled(Physical* physical) const;
	void resetScore();
	const std::vector<PlayerLevel<db_unit_level>>& getUnitLevels() const { return unitLevels; }
	const std::vector<PlayerLevel<db_building_level>>& getBuildingLevels() const { return buildingLevels; }
	void restoreUnitLevel(unsigned short id, char level);
	void restoreBuildingLevel(unsigned short id, char level);
	void restoreTechnologyLevel(unsigned short id, unsigned char level);
	unsigned char getTechnologyLevel(unsigned short id) const;
	bool canResearchTechnology(unsigned short levelId) const;
	bool canResearchTechnology(unsigned short levelId, const Building* building) const;
	bool hasTechnologyResearch(unsigned short technologyId) const;
	Building* findResearchBuilding(unsigned short levelId) const;
	bool completeTechnology(unsigned short levelId);
	float applyTechnologyAttack(float attack, const db_unit* source, const db_unit* target) const;
	float applyTechnologyAttack(float attack, const db_unit* source, const db_building* target) const;
	float applyTechnologyAttack(float attack, const db_building* source, const db_unit* target) const;
	float applyTechnologyAttack(float attack, const db_building* source, const db_building* target) const;
	float applyTechnologyResourceBonus(float bonus, const db_building* source, unsigned char resourceId) const;
	float applyTechnologyResourceBonus(float bonus, unsigned char resourceId) const;
	void refreshEffectiveLevels();
	const std::vector<unsigned char>& getTechnologyLevels() const { return technologyLevels; }
	AiHistory& getAiHistory() { return aiHistory; }
	const AiHistory& getAiHistory() const { return aiHistory; }
	AiOrchestrator& getAiOrchestrator() { return aiOrchestrator; }
	const AiOrchestrator& getAiOrchestrator() const { return aiOrchestrator; }

	unsigned getNextBuildingId() { return ++currentBuildingUId; }
	unsigned getNextUnitId() { return ++currentUnitUId; }

private:
	float technologyAgeMultiplier(const db_technology_level* level) const;

	int score = -1;

	unsigned char team;
	unsigned char id; // przed possession,resources
	bool active;
	unsigned char color;
	unsigned currentBuildingUId;
	unsigned currentUnitUId;

	db_nation* dbNation; // Must be first
	Possession* possession;
	Resources* resources;
	QueueManager queue;
	AiHistory aiHistory;
	AiOrchestrator aiOrchestrator;
	Urho3D::String name;

	std::vector<PlayerLevel<db_unit_level>> unitLevels;
	std::vector<PlayerLevel<db_building_level>> buildingLevels;
	std::vector<unsigned char> technologyLevels;
};
