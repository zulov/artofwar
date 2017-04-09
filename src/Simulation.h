#pragma once


#include "Unit.h"
#include <vector>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"
#include "ObjectManager.h"


namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation();
	void update(Input* input, float timeStep);
	void setUnits(std::vector<Unit*>* units);
	void createUnits();

private:
	void animateObjects(float timeStep);
	void moveUnits(float timeStep);
	void calculateForces();

	bool animate;

	std::vector<Unit*>* units;

	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	EnviromentStrategy* envStrategy;
	ForceStrategy* forceStrategy;
	ObjectManager * objectManager;

};
