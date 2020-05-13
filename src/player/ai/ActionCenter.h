#pragma once
#include "objects/unit/ActionParameter.h"

enum class QueueActionType : char;

namespace Urho3D {
	class Vector2;
}

class GeneralActionCommand;
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
	explicit ActionCenter(SimulationObjectManager* simulationObjectManager);

	void add(UpgradeCommand* command) const;

	void add(GeneralActionCommand* command) const;
	void add(ResourceActionCommand* command) const;
	void add(BuildingActionCommand* command) const;

	void add(UnitActionCommand* command) const;
	void add(UnitActionCommand* first, UnitActionCommand* second) const;

	void executeActions() const;
	void executeLists() const;

	bool addUnits(int number, int id, Urho3D::Vector2& position, char player) const;
	bool addBuilding(int id, Urho3D::Vector2& position, char player) const;
	bool addResource(int id, Urho3D::Vector2& position) const;

	void orderPhysical(short id, QueueActionType type, char playerId) const;

private:
	bool addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const;
	bool addBuilding(int id, Urho3D::Vector2& position, char player, int level) const;

	CreationCommandList* creation;
	UpgradeCommandList* upgrade;
	CommandList* action;
};
