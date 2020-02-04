#pragma once
#include "objects/unit/ActionParameter.h"

class SimulationObjectManager;

namespace Urho3D {
	class Vector2;
}

class ActionCommand;
class UpgradeCommandList;
class CreationCommandList;
class CreationCommand;
class UpgradeCommand;
class CommandList;

class ActionCenter {
public:
	ActionCenter(SimulationObjectManager* simulationObjectManager);
	void add(UpgradeCommand* command) const;
	//void add(CreationCommand* command) const;
	void add(ActionCommand* command) const;
	void add(ActionCommand* first, ActionCommand* second) const;
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
