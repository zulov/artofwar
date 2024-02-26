#pragma once
#include "commands/CommandList.h"
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommandList.h"

namespace Urho3D {
	class Vector2;
}

enum class QueueActionType : char;
class UnitOrder;
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

	void addUnitAction(UnitOrder* first);
	void addUnitAction(UnitOrder* first, UnitOrder* second);

	void executeActions();
	void executeLists();

	bool addUnits(int number, int id, Urho3D::Vector2 position, char player);
	bool addBuilding(int id, Urho3D::Vector2& position, char player, bool force);
	bool addResource(int id, Urho3D::Vector2& position);

private:
	bool addUnits(int number, int id, Urho3D::Vector2& position, char player, int level);
	bool addBuilding(int id, Urho3D::Vector2& position, char player, int level, bool force);

	CreationCommandList creation;
	UpgradeCommandList upgrade;
	CommandList action;
};
