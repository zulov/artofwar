#pragma once
#include "commands/CommandList.h"


namespace Urho3D {
	class Vector2;
}

class SimulationObjectManager;

class CreationCommandList : public CommandList
{
public:
	CreationCommandList();
	virtual ~CreationCommandList();
	bool addUnits(int _number, int id, Urho3D::Vector2& _position, int _player, int level);
	bool addBuilding(int id, Urho3D::Vector2& _position, int _player, int level);
	bool addResource(int id, Urho3D::Vector2& _position, int level);
	SimulationObjectManager* getManager();

protected:
	void setParemeters(AbstractCommand* command) override;
private:
	SimulationObjectManager* simulationObjectManager;
};
