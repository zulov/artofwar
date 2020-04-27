#pragma once
#include <magic_enum.hpp>
#include <vector>
#include <string>
#include "defines.h"
#include "StatsEnums.h"
#include "commands/action/UnitActionCommand.h"

//#define BASIC_INPUT_SIZE 24

#define SAVE_BATCH_SIZE 10
#define SAVE_BATCH_SIZE_MINI 4

class Building;
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

	std::string getOutput(CreationCommand* command) const;
	std::string getCreateBuildingPosOutput(CreationCommand* command) const;
	std::string getCreateUnitPosOutput(Building* building) const;
	std::string getOutput(UpgradeCommand* command) const;

	std::string getOutput(UnitActionCommand* command) const;
	std::string getOutput(ResourceActionCommand* command) const;
	std::string getOutput(BuildingActionCommand* command) const;

	static void joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output);
	void save(int i, std::vector<std::string>* array, std::string fileName) const;
	void saveBatch(int i, std::vector<std::string>* array, std::string name, int size) const;

	float* input;
	std::vector<float*> statsPerPlayer;
	float weights[magic_enum::enum_count<StatsInputType>()];

	std::vector<std::string> ordersStats[MAX_PLAYERS];

	std::vector<std::string> ordersBuildingCreateId[MAX_PLAYERS];
	std::vector<std::string> ordersBuildingCreatePos[MAX_PLAYERS];

	std::vector<std::string> ordersUnitCreateId[MAX_PLAYERS];
	std::vector<std::string> ordersUnitCreatePos[MAX_PLAYERS];
};
