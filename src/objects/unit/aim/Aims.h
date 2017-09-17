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
	Aims();
	~Aims();
	Aim* getAim(short index);
	Urho3D::Vector3* getAimPos(short index);
	void clearAims();
	bool ifReach(Urho3D::Vector3* pedestrian, int index);
	void add(Urho3D::Vector3* pos);
	bool check(int aimIndex);
	int getReferences();
	void reduce();
	void up();
private:
	std::vector<Aim*> aims;
	short int references;
};
