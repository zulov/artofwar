#pragma once

#include "commands/AbstractCommand.h"
#include <Urho3D/Math/Vector2.h>


enum class ObjectType : char;
class SimulationObjectManager;

namespace Urho3D {
	class IntVector2;
	class Vector2;
}

class CreationCommand : public AbstractCommand {
public:
	CreationCommand(ObjectType type, int id, const Urho3D::Vector2& position, int player,
	                const Urho3D::IntVector2& bucketCords, int level);
	CreationCommand(ObjectType type, int number, int id, const Urho3D::Vector2& position, int player,
	                int level);
	virtual ~CreationCommand() = default;
	void execute() override;
	void setSimulationObjectManager(SimulationObjectManager* _simulationObjectManager);
	int player;
private:
	SimulationObjectManager* simulationObjectManager;
	int id;
	int number;
	int level;
	Urho3D::Vector2 position;
	ObjectType objectType;
	Urho3D::IntVector2 bucketCords;
};
