#pragma once
#include <string>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "utils/defines.h"

constexpr char SAVE_BATCH_SIZE = 10;

struct db_basic_metric;
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
	void init();
	
	Stats(const Stats&) = delete;

	void add(GeneralActionCommand* command);
	void add(BuildingActionCommand* command);

	void add(UnitActionCommand* command);
	void add(CreationCommand* command);

	void save(bool accumulate);
private:
	void clearCounters();
	void setInputs();
	std::string getCreateBuildingPosOutput(CreationCommand* command) const;
	std::string getCreateUnitPosOutput(Building* building) const;
	std::string getPosOutput(char player, Urho3D::Vector2& pos) const;
	
	std::string getResourceIdOutput(UnitActionCommand* command) const;

	static void joinAndPush(std::vector<std::string>* array, char player, std::string input, const std::string& output,
	                        int number = 1);
	void save(int i, std::vector<std::string>* array, std::string fileName) const;
	void saveBatch(int i, std::vector<std::string>* array, std::string name, int size) const;
	void saveAll(int size);

	unsigned short workersCreatedCount[MAX_PLAYERS];
	std::vector<std::string> ifWorkersCreate[MAX_PLAYERS];
	std::vector<std::string> whereWorkersCreate[MAX_PLAYERS];

	unsigned short buildingsCreatedCount[MAX_PLAYERS];
	std::vector<std::string> ifBuildingCreate[MAX_PLAYERS];
	std::vector<std::string> whichBuildingCreate[MAX_PLAYERS];
	std::vector<std::string> whereBuildingCreate[MAX_PLAYERS];

	unsigned short unitsCreatedCount[MAX_PLAYERS];
	std::vector<std::string> ifUnitCreate[MAX_PLAYERS];
	std::vector<std::string> whichUnitCreate[MAX_PLAYERS];
	std::vector<std::string> whereUnitCreate[MAX_PLAYERS];

	std::string ifWorkersCreateInput[MAX_PLAYERS];
	std::string ifBuildingsCreatedInput[MAX_PLAYERS];
	std::string ifUnitsCreatedInput[MAX_PLAYERS];

};
