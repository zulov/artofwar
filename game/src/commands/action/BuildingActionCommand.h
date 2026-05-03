#pragma once
#include <vector>
#include "commands/PhysicalCommand.h"

enum class BuildingActionType : char;
class Building;
class Physical;

class BuildingActionCommand : public PhysicalCommand {
public:
	BuildingActionCommand(Building* building, BuildingActionType action, unsigned short id);
	BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action, unsigned short id);

	void execute() override;
private:
	std::vector<Building*> buildings;
	BuildingActionType action;
	unsigned short id;
};
