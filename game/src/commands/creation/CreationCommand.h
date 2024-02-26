#pragma once

#include <Urho3D/Math/Vector2.h>


enum class ObjectType : char;
class SimulationObjectManager;

class CreationCommand {
	friend class Stats;

public:
	CreationCommand(ObjectType type, short id, const Urho3D::IntVector2 bucketCords);
	CreationCommand(ObjectType type, short id, const Urho3D::IntVector2 bucketCords, char level, char player);
	CreationCommand(ObjectType type, short id, const Urho3D::Vector2& position, char level, char player, int number);
	virtual ~CreationCommand() = default;
	void execute(SimulationObjectManager* simulationObjectManager);

private:
	Urho3D::Vector2 position;
	Urho3D::IntVector2 bucketCords;
	int number = 1;
	ObjectType objectType;
	short id;
	char level = -1;
	char player;
};
