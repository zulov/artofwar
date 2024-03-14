#pragma once

#include <Urho3D/Math/Vector2.h>


enum class ObjectType : char;
class SimulationObjectManager;

class CreationCommand {
public:
	CreationCommand(ObjectType type, short id, const Urho3D::IntVector2 bucketCords);
	CreationCommand(ObjectType type, short id, const Urho3D::IntVector2 bucketCords, char level, char player);
	CreationCommand(ObjectType type, short id, const Urho3D::Vector2& position, char level, char player, int number);
	virtual ~CreationCommand() = default;
	void execute(SimulationObjectManager* simulationObjectManager);
	void setHp(float value) { hp = value; }

private:
	Urho3D::Vector2 position;
	Urho3D::IntVector2 bucketCords;
	int number = 1;
	ObjectType objectType;
	short id;
	char level = -1;
	char player;
	float hp = -1.0;
};
