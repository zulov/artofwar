#pragma once
#include "objects/unit/Unit.h"
#include <vector>
#include "simulation/env/Enviroment.h"
#include "simulation/force/Force.h"
#include "SimulationObjectManager.h"
#include <Urho3D/Input/Input.h>
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
	Simulation(Enviroment* _enviroment, CreationCommandList* _simCommandList);
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

	void loadEntities(SceneLoader* loader);
	void countFrame();
	void applyForce();
	void updateBuildingQueue();
	void selfAI();

	void tryToAttack(vector<Unit*>::value_type unit);
	void tryToCollect(Unit* unit);

	bool animate;
	float accumulateTime = 0;
	float maxTimeFrame = 0.05f;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	Force force;
	
	vector<Unit*>* units;
	vector<Building*>* buildings;
	vector<ResourceEntity*>* resources;

	AimContainer* aimContainer;
	SimulationInfo* simulationInfo;
	Enviroment* enviroment;
	SimulationObjectManager* simObjectManager;
	CreationCommandList* simCommandList;
	ActionCommandList* actionCommandList;
};
