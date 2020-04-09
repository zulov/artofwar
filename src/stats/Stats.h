#pragma once
#include <vector>
#include <string>
#include "defines.h"
#include "commands/action/UnitActionCommand.h"
#include "objects/building/BuildingUtils.h"

#define INPUT_STATS_SIZE 24
#define STATS_PER_PLAYER_SIZE 12
#define AI_PROPS_SIZE 3
#define SAVE_BATCH_SIZE 10
#define SAVE_BATCH_SIZE_MINI 4

struct ActionParameter;
class BuildingActionCommand;
class ResourceActionCommand;
class AbstractCommand;
class UnitActionCommand;
class UpgradeCommand;
class CreationCommand;

class Stats {
public:
	Stats();
	~Stats();
	int getScoreFor(short id) const;
	float* getInputFor(short id);
	void init();
	std::string getInputData(char player);
	//void add(UpgradeCommand* command);

	void add(ResourceActionCommand* command);
	void add(BuildingActionCommand* command);
	void add(UnitActionCommand* command);

	void add(CreationCommand* command);
	void save();
	void addBuildLevel(short id, char playerId);
private:
	void update(short id);
	void clear();
	db_building_level* Stats::getBuildingLevel(char player, int id) const;
	std::string getOutput(CreationCommand* command) const;
	std::string getBuildingOutput(CreationCommand* command) const;
	std::string getOutput(UpgradeCommand* command) const;

	std::string getOutput(UnitActionCommand* command) const;
	std::string getOutput(ResourceActionCommand* command) const;
	std::string getOutput(BuildingActionCommand* command) const;

	void joinAndPush(std::vector<std::string>* array, char player, std::string input, std::string& output);
	void saveBatch(int i, std::vector<std::string>* array, int size);

	float* input;
	std::vector<float*> statsPerPlayer;
	float weights[STATS_PER_PLAYER_SIZE];
	std::vector<std::string> ordersStats[MAX_PLAYERS];
	std::vector<std::string> ordersBuildingCreate[MAX_PLAYERS];
};
