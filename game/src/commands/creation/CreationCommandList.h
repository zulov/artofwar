#pragma once
#include <vector>

namespace Urho3D {
	class IntVector2;
	class Vector2;
}

class SimulationObjectManager;
class CreationCommand;

class CreationCommandList  {
public:
	explicit CreationCommandList(SimulationObjectManager* simulationObjectManager);
	virtual ~CreationCommandList() = default;
	CreationCommand* addUnits(int number, short id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuilding(short id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuildingForce(short id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addResource(short id, Urho3D::IntVector2& cords) const;

	void add(CreationCommand* command);
	void execute();
private:
	std::vector<CreationCommand*> commands;
	SimulationObjectManager* simulationObjectManager;
};
