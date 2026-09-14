#pragma once
#include <vector>
#include "commands/PhysicalCommand.h"

enum class BuildingActionType : unsigned char;
class Building;
class Physical;
struct PendingCommandSaveData;

class BuildingActionCommand : public PhysicalCommand {
public:
	BuildingActionCommand(Building* building, BuildingActionType action, unsigned short id);
	BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action, unsigned short id);

	void execute() override;
	PendingCommandSaveData saveState(unsigned short order) const override;

private:
	std::vector<Building*> buildings;
	BuildingActionType action;
	unsigned short id;
};
