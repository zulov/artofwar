#pragma once
#include <magic_enum.hpp>
#include <string>
#include <vector>
#include "StatsEnums.h"
#include "commands/action/UnitActionCommand.h"
#include "utils/defines.h"

#define SAVE_BATCH_SIZE 10
#define SAVE_BATCH_SIZE_MINI 4

struct ActionParameter;
class GeneralActionCommand;
class Building;
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
	float* getBasicInput(short id);
	void init();
	std::string getInputData(char player);

	void add(GeneralActionCommand* command);
	void add(ResourceActionCommand* command);
	void add(BuildingActionCommand* command);
	
	void add(UnitActionCommand* command);

	void add(CreationCommand* command);
	
	void save();
private:
	void update(short id);
	void clear();

	std::string getOutput(CreationCommand* command) const;
	std::string getCreateBuildingPosOutput(CreationCommand* command) const;
	std::string getCreateUnitPosOutput(Building* building) const;
	std::string getLevelUpUnitPosOutput(Building* building)const;
	std::string getOutput(UpgradeCommand* command) const;

	std::string getOutput(UnitActionCommand* command) const;
	std::string getOutput(ResourceActionCommand* command) const;
	std::string getOutput(BuildingActionCommand* command) const;
	std::string getOutput(GeneralActionCommand* command) const;
	
	std::string getOutput(StatsOutputType stat) const;

	static void joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output);
	void save(int i, std::vector<std::string>* array, std::string fileName) const;
	void saveBatch(int i, std::vector<std::string>* array, std::string name, int size) const;
	void saveAll(int big, int small);

	float* input;
	std::vector<float*> statsPerPlayer;
	float weights[magic_enum::enum_count<StatsInputType>()];

	std::vector<std::string> mainOrder[MAX_PLAYERS];

	std::vector<std::string> buildingCreateId[MAX_PLAYERS];
	std::vector<std::string> buildingCreatePos[MAX_PLAYERS];

	std::vector<std::string> unitCreateId[MAX_PLAYERS];
	std::vector<std::string> unitCreatePos[MAX_PLAYERS];

	std::vector<std::string> unitOrderId[MAX_PLAYERS];

	std::vector<std::string> buildLevelUpId[MAX_PLAYERS];
	std::vector<std::string> unitUpgradeId[MAX_PLAYERS];
	std::vector<std::string> unitLevelUpPos[MAX_PLAYERS];
};
