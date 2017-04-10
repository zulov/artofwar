#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>
#include "Entity.h"

#include "Aim.h"
class Entity;

class Aims
{
public:
	Aims();
	~Aims();
	Aim* getAim();
	Vector3 *getAimPos();
	bool check(Urho3D::Vector3* pedestrian);
	void add(Entity* entity);
private:
	std::vector<Aim*>* aims;
	int index;
};
