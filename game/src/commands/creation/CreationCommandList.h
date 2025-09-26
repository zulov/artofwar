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
	~CreationCommandList() = default;
	CreationCommand* addUnits(unsigned number, short id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuilding(short id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuildingForce(short id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addResource(short id, const Urho3D::IntVector2& cords) const;

	void add(CreationCommand* command);
	void execute();
private:
	std::vector<CreationCommand*> commands;
	SimulationObjectManager* simulationObjectManager;
};
