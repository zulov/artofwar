#pragma once
#include "objects/unit/Unit.h"
#include <vector>
#include <ctime>
#include "simulation/env/EnviromentStrategy.h"
#include "simulation/force/ForceStrategy.h"
#include "SimulationObjectManager.h"
#include "commands/SimulationCommandList.h"
#include "commands/CommandList.h"
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>
#include "commands/ActionCommandList.h"
#include "objects/unit/aim/AimContainer.h"
#include "SimulationInfo.h"

class SimulationCommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(EnviromentStrategy* _enviromentStrategy, SimulationCommandList* _simCommandList, SimulationObjectManager* _simObjectManager);
	void selftAI();
	void update(Input* input, float timeStep);
	void createUnits();
	float updateTime(float timeStep);
	void countFrame();
	void applyForce();
	void updateBuildingQueue();
	int getUnitsNumber();
	SimulationInfo* getInfo();
	void updateEnviroment();
	void dispose();
	void performAction();

private:
	void moveUnits(float timeStep);
	void moveUnitsAndCheck(float timeStep);
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
	SimulationInfo* simulationInfo;
	EnviromentStrategy* envStrategy;
	ForceStrategy* forceStrategy;
	SimulationObjectManager* simObjectManager;
	SimulationCommandList* simCommandList;
	ActionCommandList* actionCommandList;
};
