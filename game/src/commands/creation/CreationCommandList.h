#pragma once
#include <vector>

namespace Urho3D {
	class Vector2;
}

class SimulationObjectManager;
class CreationCommand;

class CreationCommandList  {
public:
	explicit CreationCommandList(SimulationObjectManager* simulationObjectManager);
	virtual ~CreationCommandList() = default;
	CreationCommand* addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuilding(int id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuildingForce(int id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addResource(int id, Urho3D::Vector2& position) const;

	void add(CreationCommand* command);
	void execute();
private:
	std::vector<CreationCommand*> commands;
	SimulationObjectManager* simulationObjectManager;
};
