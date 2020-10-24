#pragma once
#include "commands/AbstractCommand.h"


namespace Urho3D {
	class Vector2;
}

class UnitOrder;
class Physical;
enum class UnitAction : char;

class UnitActionCommand : public AbstractCommand {
	friend class Stats;
public:
	UnitActionCommand(UnitOrder* order, char player);
	~UnitActionCommand() override;

	void execute() override;
protected:
	UnitOrder* order;
};
