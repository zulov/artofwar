#pragma once

#include "Main.h"
#include "Unit.h"
#include <vector>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"
#include "Benchmark.h"
#include "Controls.h"

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation : public Main {
	URHO3D_OBJECT(Simulation, Main);

public:
	Simulation(Context* context);
	virtual void Start();

	void createGround();

protected:

private:	
	void clickLeft();
	void clickRight();
	void createUnits(int size, double space);
	void SubscribeToEvents();
	void moveCamera(float timeStep);
	void AnimateObjects(float timeStep);
	void moveUnits(float timeStep);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void calculateForces();
	void reset();
	void resetUnits();
	void updateHud(float timeStep);	
	bool animate;

	std::vector<Unit*> *units;

	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	EnviromentStrategy *envStrategy;
	ForceStrategy * forceStrategy;

	Text* fpsText;
	Benchmark * benchmark;
	int edgeSize = 20;
	double spaceSize = 1.5;

	Controls * controls;


};
