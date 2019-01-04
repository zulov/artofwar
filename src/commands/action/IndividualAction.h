#pragma once
#include "ActionCommand.h"
#include "objects/unit/aim/Aims.h"


class IndividualAction 
{
public:
	IndividualAction(Physical* entity, FutureOrder& futureAim, bool append = false);
	static Aim* createAim(Unit *unit, FutureOrder& nextAim);
	void addAim();
	~IndividualAction();
private:
	// void addTargetAim(Urho3D::Vector2* to, bool append) override;
	// void addChargeAim(Urho3D::Vector2* charge, bool append) override;
	// void addFollowAim(const Physical* toFollow, bool append) override;
	// void addAttackAim(const Physical* physical, bool append) override;
	// void addDeadAim() override;	
	// void addDefendAim() override;
	Physical* entity; //TODO czy to moze byc UNit?

};
