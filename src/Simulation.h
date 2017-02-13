#pragma once

#include "Main.h"
#include "Unit.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"
#include "Benchmark.h"
#include "Hud.h"

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation : public Main {
	URHO3D_OBJECT(Simulation, Main);

public:
	Simulation(Context* context);
	virtual void Start();

protected:

private:	
	void CreateScene();
	void createCamera();
	void createLight();
	void createZone();
	void createUnits(int size, double space);
	void CreateStaticHud();
	void SubscribeToEvents();
	void moveCamera(float timeStep);
	void AnimateObjects(float timeStep);
	void moveUnits(float timeStep);
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

	bool animate;

	std::vector<Unit*> *units;

	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	void calculateForces();
	EnviromentStrategy *envStrategy;
	ForceStrategy * forceStrategy;
	void reset();
	void resetUnits();
private:
	void updateHud(float timeStep);
	Text* fpsText;
	Benchmark * benchmark;
	int edgeSize = 25;
	double spaceSize = 1.5;
	Hud * hud;

};
