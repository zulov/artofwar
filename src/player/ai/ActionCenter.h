#pragma once
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommandList.h"

enum class QueueActionType : char;

namespace Urho3D {
	class Vector2;
}

class GeneralActionCommand;
class BuildingActionCommand;
class ResourceActionCommand;
class SimulationObjectManager;
class UnitActionCommand;
class CreationCommand;
class UpgradeCommand;

class ActionCenter {
public:
	explicit ActionCenter(SimulationObjectManager* simulationObjectManager);

	void add(UpgradeCommand* command);

	void add(GeneralActionCommand* command);
	void add(ResourceActionCommand* command);
	void add(BuildingActionCommand* command);

	void add(UnitActionCommand* command);
	void add(UnitActionCommand* first, UnitActionCommand* second);

	void executeActions();
	void executeLists();

	bool addUnits(int number, int id, Urho3D::Vector2& position, char player);
	bool addBuilding(int id, Urho3D::Vector2& position, char player);
	bool addResource(int id, Urho3D::Vector2& position);

private:
	bool addUnits(int number, int id, Urho3D::Vector2& position, char player, int level);
	bool addBuilding(int id, Urho3D::Vector2& position, char player, int level);

	CreationCommandList creation;
	UpgradeCommandList upgrade;
	CommandList action;
};
