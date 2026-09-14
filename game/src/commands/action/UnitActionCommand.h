#pragma once
#include "commands/PhysicalCommand.h"

namespace Urho3D {
	class Vector2;
}

class UnitOrder;
class Physical;
enum class UnitAction : char;
struct PendingCommandSaveData;

class UnitActionCommand : public PhysicalCommand {
public:
	UnitActionCommand(UnitOrder* order);
	~UnitActionCommand() override = default;

	void execute() override;
	PendingCommandSaveData saveState(unsigned short order) const override;

protected:
	UnitOrder* order;
};
