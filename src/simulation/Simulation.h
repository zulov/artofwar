#pragma once
#include "SimulationInfo.h"
#include "SimulationObjectManager.h"
#include "commands/upgrade/UpgradeCommandList.h"
#include "hud/window/main_menu/new_game/NewGameForm.h"
#include "objects/unit/Unit.h"
#include "scene/save/SceneSaver.h"
#include "simulation/env/Enviroment.h"
#include "simulation/force/Force.h"
#include <vector>


class SceneLoader;
class CreationCommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(Enviroment* _enviroment, CreationCommandList* _creationCommandList);
	~Simulation();
	SimulationInfo* update(float timeStep);
	void executeLists();
	void initScene(SceneLoader& loader);
	void initScene(NewGameForm* form);

	void dispose();
	void save(SceneSaver& saver);
	void changeCoef(int i, int wheel);
	void changeColorMode(ColorMode _colorMode);

private:
	void moveUnits(float timeStep);
	void moveUnitsAndCheck(float timeStep);
	void calculateForces();
	void performStateAction();
	void handleTimeInFrame(float timeStep);
	float updateTime(float timeStep);

	void loadEntities(NewGameForm* form);
	void loadEntities(SceneLoader& loader) const;
	void countFrame();
	void applyForce();
	void levelUp(QueueElement* done);
	void updateBuildingQueues(float time);
	void updateQueues();
	void selfAI();
	void addTestEntities();

	void tryToAttack(vector<Unit*>::value_type unit);
	void tryToCollect(Unit* unit);

	float accumulateTime = 0;
	float maxTimeFrame = 0.05f;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	ColorMode currentColor = ColorMode::BASIC;
	Force force;

	vector<Unit*>* units;
	vector<Building*>* buildings;
	vector<ResourceEntity*>* resources;

	SimulationInfo* simulationInfo;
	Enviroment* enviroment;
	SimulationObjectManager* simObjectManager;
	CreationCommandList* creationCommandList;
	UpgradeCommandList* levelsCommandList;
	CommandList* actionCommandList;
};
