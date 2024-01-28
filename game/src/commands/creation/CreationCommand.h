#pragma once

#include "commands/AbstractCommand.h"
#include <Urho3D/Math/Vector2.h>


enum class ObjectType : char;
class SimulationObjectManager;

class CreationCommand : public AbstractCommand {
	friend class Stats;

public:
	CreationCommand(ObjectType type, int id, const Urho3D::IntVector2 bucketCords);
	CreationCommand(ObjectType type, int id, const Urho3D::IntVector2 bucketCords, char level, char player);
	CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, char level, char player, int number);
	virtual ~CreationCommand() = default;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);

private:
	SimulationObjectManager* simulationObjectManager;
	Urho3D::Vector2 position;
	Urho3D::IntVector2 bucketCords;
	int number = 1;
	ObjectType objectType;
	char id;
	char level = -1;
};
