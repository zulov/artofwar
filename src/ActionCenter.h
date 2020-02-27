#pragma once
#include "objects/unit/ActionParameter.h"


namespace Urho3D {
	class Vector2;
}

class BuildingActionCommand;
class ResourceActionCommand;
class SimulationObjectManager;
class UnitActionCommand;
class UpgradeCommandList;
class CreationCommandList;
class CreationCommand;
class UpgradeCommand;
class CommandList;

class ActionCenter {
public:
	ActionCenter(SimulationObjectManager* simulationObjectManager);

	void add(UpgradeCommand* command) const;

	void add(ResourceActionCommand* command) const;
	void add(BuildingActionCommand* command) const;

	void add(UnitActionCommand* command) const;
	void add(UnitActionCommand* first, UnitActionCommand* second) const;

	void executeActions() const;
	void executeLists() const;
	bool addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const;
	bool addBuilding(int id, Urho3D::Vector2& position, char player, int level) const;
	bool addResource(int id, Urho3D::Vector2& position, int level) const;
	void orderPhysical(short id, const ActionParameter& parameter, char playerId) const;

private:

	CreationCommandList* creation;
	UpgradeCommandList* upgrade;
	CommandList* action;
};
