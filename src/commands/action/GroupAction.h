#pragma once
#include "ActionCommand.h"
#include <vector>

class GroupAction : public ActionCommand
{
public:
	GroupAction(std::vector<Physical*>* entities, UnitOrder action, Urho3D::Vector2* parameter, bool append = false);
	GroupAction(std::vector<Physical*>* entities, UnitOrder action, Physical* paremeter, bool append = false);
	GroupAction(std::vector<Physical*>* entities, UnitOrder action, bool append = false);
	~GroupAction();
private:
	void addTargetAim(Urho3D::Vector2* to, bool append) override;
	void addChargeAim(Urho3D::Vector2* charge, bool append) override;
	void addFollowAim(const Physical* toFollow, bool append) override;
	void addDeadAim() override;
	void addDefendAim() override;
	std::vector<Physical*>* entities;
};
