#pragma once
#include "objects/unit/Unit.h"
#include <vector>
#include <ctime>
#include "simulation/env/Enviroment.h"
#include "simulation/force/Force.h"
#include "SimulationObjectManager.h"
#include "commands/CreationCommandList.h"
#include "commands/CommandList.h"
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>
#include "commands/ActionCommandList.h"
#include "objects/unit/aim/AimContainer.h"
#include "SimulationInfo.h"

class CreationCommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(Enviroment* _enviromentStrategy, CreationCommandList* _simCommandList, SimulationObjectManager* _simObjectManager);

	void update(Input* input, float timeStep);

	int getUnitsNumber();
	SimulationInfo* getInfo();
	void dispose();
	
private:
	void moveUnits(float timeStep);
	void moveUnitsAndCheck(float timeStep);
	void calculateForces();
	void performAction();
	float updateTime(float timeStep);
	void updateEnviroment();
	void createUnits();
	void countFrame();
	void applyForce();
	void updateBuildingQueue();
	void selftAI();

	void tryToAttack(vector<Unit*>::value_type unit);
	void tryToCollect(Unit* unit);

	bool animate;
	double accumulateTime = 0;
	double maxTimeFrame = 0.05;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	vector<Unit*>* units;
	vector<Building*>* buildings;
	vector<ResourceEntity*>* resources;

	AimContainer* aimContainer;
	SimulationInfo* simulationInfo;
	Enviroment* envStrategy;
	Force* forceStrategy;
	SimulationObjectManager* simObjectManager;
	CreationCommandList* simCommandList;
	ActionCommandList* actionCommandList;
};
