#pragma once

#include "commands/AbstractCommand.h"
#include <Urho3D/Math/Vector2.h>


enum class ObjectType : char;
class SimulationObjectManager;

class CreationCommand : public AbstractCommand {
	friend class Stats;
public:
	CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, char player,
	                const Urho3D::IntVector2& bucketCords, int level);
	CreationCommand(ObjectType type, int number, int id, const Urho3D::Vector2& position,
	                char player, int level);
	virtual ~CreationCommand() = default;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
private:
	SimulationObjectManager* simulationObjectManager;
	int id;
	int number;
	int level;
	Urho3D::Vector2 position;
	ObjectType objectType;
	Urho3D::IntVector2 bucketCords;
};
