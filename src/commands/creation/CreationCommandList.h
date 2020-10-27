#pragma once
#include "commands/CommandList.h"


namespace Urho3D {
	class Vector2;
}

class SimulationObjectManager;
class CreationCommand;

class CreationCommandList : public CommandList {
public:
	explicit CreationCommandList(SimulationObjectManager* simulationObjectManager);
	virtual ~CreationCommandList() = default;
	CreationCommand* addUnits(int number, int id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addBuilding(int id, Urho3D::Vector2& position, char player, int level) const;
	CreationCommand* addResource(int id, Urho3D::Vector2& position, int level) const;

protected:
	void setParameters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
