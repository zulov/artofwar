#pragma once
#include <vector>
#include "commands/PhysicalCommand.h"

enum class BuildingActionType : char;
class Building;
class Physical;

class BuildingActionCommand : public PhysicalCommand {
public:
	BuildingActionCommand(Building* building, BuildingActionType action, short id);
	BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action, short id);

	void execute() override;
private:
	short id;
	BuildingActionType action;
	std::vector<Building*> buildings;
};
