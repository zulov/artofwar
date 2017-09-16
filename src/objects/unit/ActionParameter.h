#pragma once

#include "aim/Aims.h"

class Physical;

class ActionParameter
{
public:
	ActionParameter();
	~ActionParameter();
	Aims* getAims();
	Physical * getFollowTo();
	void setAims(Aims* _aims);
	void setVector(Urho3D::Vector3* _aim);
	void setFollowTo(Physical * physical);
private:
	Urho3D::Vector3* vector;
	Aims * aims;
	Physical * followTo;
};

