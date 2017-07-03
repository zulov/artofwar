#pragma once
#include "Unit.h"
#include <vector>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"
#include "SimulationObjectManager.h"
#include "SimulationCommandList.h"
#include "CommandList.h"
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>
#include "ActionCommandList.h"
#include "AimContainer.h"

class SimulationCommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(EnviromentStrategy* _enviromentStrategy, SimulationCommandList* _simCommandList, SimulationObjectManager* _simObjectManager);
	void action();
	void update(Input* input, float timeStep);
	void createUnits();
	float updateTime(float timeStep);
	void countFrame();
	void applyForce();

private:
	void moveUnits(float timeStep);
	void calculateForces();
	bool animate;
	double accumulateTime = 0;
	double maxTimeFrame = 0.05;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	vector<Unit*>* units;
	vector<Building*>* buildings;
	vector<ResourceEntity*>* resources;
	AimContainer* aimContainer;
	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	EnviromentStrategy* envStrategy;
	ForceStrategy* forceStrategy;
	SimulationObjectManager* simObjectManager;
	SimulationCommandList* simCommandList;
	ActionCommandList* actionCommandList;
};
