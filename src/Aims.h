#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>
#include "Entity.h"

#include "Aim.h"
class Entity;

class Aims
{
public:
	Aims(int _references);
	~Aims();
	Aim* getAim(int index);
	Vector3 *getAimPos(int index);
	void clearAims();
	bool ifReach(Urho3D::Vector3* pedestrian, int index);
	void add(Entity* entity);
	bool check(int aimIndex);
	int getReferences();
private:
	std::vector<Aim*>* aims;
	int references;
};
