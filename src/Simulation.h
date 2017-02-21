#pragma once


#include "Unit.h"
#include <vector>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"


namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
	//URHO3D_OBJECT(Simulation, Main);

public:
	Simulation(Context* context);
	void update(Input* input, float timeStep);
	void setUnits(std::vector<Unit*>* units);

private:
	void animateObjects(float timeStep);
	void moveUnits(float timeStep);

	void calculateForces();
	void reset();
	void resetUnits();

	bool animate;

	std::vector<Unit*>* units;

	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	EnviromentStrategy* envStrategy;
	ForceStrategy* forceStrategy;


};
