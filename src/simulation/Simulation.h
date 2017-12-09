#pragma once
#include "objects/unit/Unit.h"
#include <vector>
#include "simulation/env/Enviroment.h"
#include "simulation/force/Force.h"
#include "SimulationObjectManager.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/DebugNew.h>
#include "objects/unit/aim/AimContainer.h"
#include "SimulationInfo.h"
#include "scene/save/SceneSaver.h"
#include "commands/action/ActionCommandList.h"


class SceneLoader;
class CreationCommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(Enviroment* _enviromentStrategy, CreationCommandList* _simCommandList);
	~Simulation();
	void update(Input* input, float timeStep);
	void initScene(SceneLoader* loader);
	int getUnitsNumber();
	SimulationInfo* getInfo();
	void dispose();
	void save(SceneSaver* saver);

private:
	void moveUnits(float timeStep);
	void moveUnitsAndCheck(float timeStep);
	void calculateForces();
	void performAction();
	float updateTime(float timeStep);
	void updateEnviroment();
	void createUnits(SceneLoader* loader);
	void countFrame();
	void applyForce();
	void updateBuildingQueue();
	void selfAI();

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
	Enviroment* enviroment;
	Force* force;
	SimulationObjectManager* simObjectManager;
	CreationCommandList* simCommandList;
	ActionCommandList* actionCommandList;
};
