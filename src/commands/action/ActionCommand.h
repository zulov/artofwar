#pragma once
#include "commands/AbstractCommand.h"
#include "objects/unit/aim/order/FutureOrder.h"


namespace Urho3D {
	class Vector2;
}

class Physical;
enum class UnitOrder : char;

class ActionCommand : public AbstractCommand
{
public:
	ActionCommand(FutureOrder* futureAim);
	~ActionCommand() override;

	void execute() override;
protected:
	FutureOrder* futureAim;
};
