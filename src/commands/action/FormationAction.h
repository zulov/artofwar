#pragma once
#include "ActionCommand.h"

class Formation;

class FormationAction : public ActionCommand
{
public:
	FormationAction(Formation* formation, UnitOrder action, const Physical* physical, Urho3D::Vector2* vector, bool append = false);
	~FormationAction();
private:
	void addTargetAim(Urho3D::Vector2* to, bool append) override;
	void addChargeAim(Urho3D::Vector2* charge, bool append) override;
	void addFollowAim(const Physical* toFollow, bool append) override;
	void addAttackAim(const Physical* toAttack, bool append) override;
	void addDeadAim() override;
	void addDefendAim() override;
	Formation* formation;
};
