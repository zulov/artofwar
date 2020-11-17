#pragma once
#include <vector>
#include "commands/AbstractCommand.h"

enum class BuildingActionType : char;
class Building;
class Physical;

class BuildingActionCommand : public AbstractCommand {
	friend class Stats;
public:
	BuildingActionCommand(Building* building, BuildingActionType action, short id, char player);
	BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action, short id, char player);

	void clean() override;
	bool expired() override;
	void execute() override;
private:
	short id;
	BuildingActionType action;
	std::vector<Building*> buildings;
};
