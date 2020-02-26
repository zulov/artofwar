#pragma once
#include "commands/AbstractCommand.h"
#include "objects/order/FutureOrder.h"


namespace Urho3D {
	class Vector2;
}

class Physical;
enum class UnitAction : char;

class UnitActionCommand : public AbstractCommand {
	friend class Stats;
public:
	UnitActionCommand(FutureOrder* futureAim, char player);
	~UnitActionCommand() override;
	
	void execute() override;
protected:
	FutureOrder* futureAim;
};
