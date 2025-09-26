#pragma once

#include <Urho3D/Math/Vector2.h>


enum class ObjectType : char;
class SimulationObjectManager;

class CreationCommand {
public:
	CreationCommand(ObjectType type, short id, const Urho3D::IntVector2& bucketCords);
	CreationCommand(ObjectType type, short id, const Urho3D::IntVector2& bucketCords, char level, char player);
	CreationCommand(ObjectType type, short id, const Urho3D::Vector2& position, char level, char player, unsigned number);
	~CreationCommand() = default;
	void execute(SimulationObjectManager* simulationObjectManager);
	void setHp(float value) { hp = value; }

private:
	Urho3D::Vector2 position; // TODO Union
	Urho3D::IntVector2 bucketCords; // TODO Union
	unsigned number = 1;
	float hp = -1.f;
	short id;
	ObjectType objectType;
	char level = -1;
	char player;
};
