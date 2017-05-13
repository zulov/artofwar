#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>
#include "Aim.h"
#include "utils.h"
#include <Urho3D/Graphics/StaticModel.h>

class Entity;

class Aims
{
public:
	Aims(int _references);
	~Aims();
	Aim* getAim(int index);
	Urho3D::Vector3* getAimPos(int index);
	void clearAims();
	bool ifReach(Urho3D::Vector3* pedestrian, int index);
	void add(Urho3D::Vector3* pos);
	bool check(int aimIndex);
	int getReferences();
	void reduce();
	void up();
private:
	std::vector<Aim*>* aims;
	int references;
};
