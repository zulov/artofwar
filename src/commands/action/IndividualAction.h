#pragma once
#include "ActionCommand.h"

class IndividualAction : public ActionCommand
{
public:
	IndividualAction(Physical* entity, OrderType action, const Physical* paremeter, bool append = false);
	IndividualAction(Physical* entity, OrderType action, const Urho3D::Vector2& vector, bool append = false);
	~IndividualAction();
private:
	void addTargetAim(Urho3D::Vector2* to, bool append) override;
	void addChargeAim(Urho3D::Vector2* charge, bool append) override;
	void addFollowAim(const Physical* toFollow, bool append) override;
	Physical* entity;
};
