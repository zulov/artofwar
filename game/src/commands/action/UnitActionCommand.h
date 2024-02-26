#pragma once
#include "commands/PhysicalCommand.h"

namespace Urho3D {
	class Vector2;
}

class UnitOrder;
class Physical;
enum class UnitAction : char;

class UnitActionCommand : public PhysicalCommand {
	friend class Stats;
public:
	UnitActionCommand(UnitOrder* order);
	~UnitActionCommand() override = default;

	void execute() override;
protected:
	UnitOrder* order;
};
