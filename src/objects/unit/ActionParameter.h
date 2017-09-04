#pragma once

#include "aim/Aims.h"

class Physical;

class ActionParameter
{
public:
	ActionParameter();
	~ActionParameter();
	Aims* getAims();
	Urho3D::Vector3 * getAimPosition();
	Physical * getFollowTo();
	void setAims(Aims* _aims);
	void setAimPosition(Urho3D::Vector3* _aim);
	void setFollowTo(Physical * physical);
private:
	Urho3D::Vector3* aimPosition;
	Aims * aims;
	Physical * followTo;
};

