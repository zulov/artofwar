#pragma once
#include "Aims.h"

class ActionParameter
{
public:
	ActionParameter();
	~ActionParameter();
	Aims* getAims();
	Urho3D::Vector3 * getAimPosition();
	void setAims(Aims* _aims);
	void setAimPosition(Urho3D::Vector3* _aim);
private:
	Urho3D::Vector3* aimPosition;
	Aims * aims;
};

