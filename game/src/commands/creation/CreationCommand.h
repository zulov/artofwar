#pragma once

#include <Urho3D/Math/Vector2.h>

enum class ObjectType : char;
class SimulationObjectManager;
struct PendingCommandSaveData;

class CreationCommand {
public:
	CreationCommand(ObjectType type, unsigned short id, const Urho3D::UShortVector2& bucketCords);
	CreationCommand(ObjectType type, unsigned short id, const Urho3D::UShortVector2& bucketCords, char level,
					unsigned char playerId);
	CreationCommand(ObjectType type, unsigned short id, const Urho3D::Vector2& position, char level, unsigned char playerId,
					unsigned number);
	~CreationCommand() = default;
	void execute(SimulationObjectManager* simulationObjectManager);
	void setHp(float value) { hp = value; }
	PendingCommandSaveData saveState(unsigned short order) const;

private:
	union {
		Urho3D::Vector2 position;
		Urho3D::UShortVector2 bucketCords;
	};
	unsigned number = 1;
	float hp = -1.f;
	unsigned short id;
	ObjectType objectType;
	unsigned char playerId;
};
