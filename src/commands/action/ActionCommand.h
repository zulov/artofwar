#pragma once
#include "commands/AbstractCommand.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/Aims.h"
#include "objects/unit/aim/order/FutureOrder.h"


namespace Urho3D {
	class Vector2;
}

class Physical;
enum class UnitOrder : char;

class ActionCommand : public AbstractCommand
{
public:
	ActionCommand(FutureOrder &futureAim, bool append);
	virtual ~ActionCommand();

	void execute() override;
protected:
	virtual void addAim() =0;

	static ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	static ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow);
	static ActionParameter getChargeAim(Urho3D::Vector2* charge);

	// virtual void addTargetAim(Urho3D::Vector2* to, bool append) =0;
	// virtual void addFollowAim(const Physical* toFollow, bool append) =0;
	// virtual void addChargeAim(Urho3D::Vector2* charge, bool append) =0;
	// virtual void addDeadAim() =0;
	//
	// virtual void addAttackAim(const Physical* physical, bool append) =0;
	// virtual void addDefendAim() =0;
	
	FutureOrder futureAim;
	bool append;
};
