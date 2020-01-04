#pragma once
#include "stats/Stats.h"
#include "commands/upgrade/UpgradeCommand.h"


namespace Urho3D {
	class Vector2;
}

class ActionCommand;
class UpgradeCommandList;
class CreationCommandList;
class CreationCommand;
class CommandList;

class ActionCenter {
public:
	ActionCenter(SimulationObjectManager* simulationObjectManager);
	void add(UpgradeCommand* command) const;
	void add(CreationCommand* command) const;
	void add(ActionCommand* command) const;
	void add(ActionCommand* first, ActionCommand* second) const;
	void executeActions() const;
	void executeLists() const;
	bool addUnits(int number, int id, Urho3D::Vector2& position, int player, int level) const;
	bool addBuilding(int id, Urho3D::Vector2& position, int player, int level) const;
	bool addResource(int id, Urho3D::Vector2& position, int level);

private:
	Stats* stats;
	CreationCommandList* creation;
	UpgradeCommandList* upgrade;
	CommandList* action;
};
