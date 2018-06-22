#pragma once
#include "commands/AbstractCommand.h"
#include "objects/unit/ActionParameter.h"


namespace Urho3D {
	class Vector2;
}

class Physical;
enum class UnitOrder : char;

class ActionCommand : public AbstractCommand
{
public:
	ActionCommand(UnitOrder action, const Physical* physical, Urho3D::Vector2* vector, bool append);
	virtual ~ActionCommand();

	void execute() override;
protected:
	ActionParameter getTargetAim(int startIdx, Urho3D::Vector2& to, bool append);
	ActionParameter getFollowAim(const Physical* toFollow, bool append);
	ActionParameter getChargeAim(Urho3D::Vector2* charge, bool append);

	virtual void addTargetAim(Urho3D::Vector2* to, bool append) =0;
	virtual void addFollowAim(const Physical* toFollow, bool append) =0;
	virtual void addChargeAim(Urho3D::Vector2* charge, bool append) =0;

	UnitOrder action;
	Urho3D::Vector2* vector;
	const Physical* toFollow;

	bool append;
};
