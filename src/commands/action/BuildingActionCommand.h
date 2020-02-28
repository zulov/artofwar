#pragma once
#include <vector>
#include "objects/Physical.h"
#include "commands/AbstractCommand.h"

enum class BuildingActionType : char;
class Building;

class BuildingActionCommand : public AbstractCommand {
	friend class Stats;
public:
	BuildingActionCommand(Building* building, BuildingActionType action, short id, char player);
	BuildingActionCommand(std::vector<Physical*>* buildings, BuildingActionType action, short id, char player);

	void clean() override;
	bool expired() override;

	void execute() override;
private:
	short id;
	BuildingActionType action;
	std::vector<Building*> buildings;
};
