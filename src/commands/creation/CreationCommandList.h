#pragma once
#include "commands/CommandList.h"


namespace Urho3D {
	class Vector2;
}

class SimulationObjectManager;

class CreationCommandList : public CommandList {
public:
	CreationCommandList(SimulationObjectManager* simulationObjectManager);
	virtual ~CreationCommandList();
	bool addUnits(int number, int id, Urho3D::Vector2& position, char player, int level);
	bool addBuilding(int id, Urho3D::Vector2& position, char player, int level);
	bool addResource(int id, Urho3D::Vector2& position, int level);

protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
