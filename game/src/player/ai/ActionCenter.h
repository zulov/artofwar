#pragma once
#include <unordered_map>
#include "commands/CommandList.h"
#include "commands/creation/CreationCommandList.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "scene/load/RuntimeSaveData.h"

namespace Urho3D {
	class Vector2;
}

class UnitOrder;
class GeneralActionCommand;
class BuildingActionCommand;
class ResourceActionCommand;
class SimulationObjectManager;
class UnitActionCommand;
class CreationCommand;
class UpgradeCommand;
class Physical;

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
	void createAndUpgrade();

	bool addUnits(unsigned number, short id, Urho3D::Vector2 position, char player);
	bool addBuilding(short id, Urho3D::Vector2& position, char player, bool force);
	bool addResource(short id, int index, float hp = -1.f);
	std::vector<PendingCommandSaveData> saveState() const;
	void loadState(const std::vector<PendingCommandSaveData>& state,
				   const std::unordered_map<unsigned, Physical*>& byUid);

private:
	bool addUnits(unsigned number, short id, Urho3D::Vector2& position, char player, int level);
	bool addBuilding(short id, Urho3D::Vector2& position, char player, int level, bool force);

	CreationCommandList creation;
	UpgradeCommandList upgrade;
	CommandList action;
};
