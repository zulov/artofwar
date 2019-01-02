#pragma once
#include "ActionCommand.h"
#include <vector>

class GroupAction : public ActionCommand
{
public:
	GroupAction(std::vector<Physical*>* entities, FutureOrder &futureAim, bool append = false);
	~GroupAction();
private:
	void addAim() override;
	// void addTargetAim(Urho3D::Vector2* to, bool append) override;
	// void addChargeAim(Urho3D::Vector2* charge, bool append) override;
	// void addFollowAim(const Physical* toFollow, bool append) override;
	// void addAttackAim(const Physical* physical, bool append) override;
	// void addDeadAim() override;
	// void addDefendAim() override;
	std::vector<Physical*>* entities;
};
