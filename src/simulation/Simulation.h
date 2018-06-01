#pragma once
#include <vector>
#include "SimulationInfo.h"
#include "force/Force.h"
#include "objects/building/Building.h"
#include "SimulationObjectManager.h"
#include "commands/upgrade/UpgradeCommandList.h"


class Unit;
class QueueElement;
enum class ColorMode : char;
struct NewGameForm;
class Enviroment;
class SceneSaver;
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
	void executeLists() const;
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

	void loadEntities(NewGameForm* form) const;
	void loadEntities(SceneLoader& loader) const;
	void countFrame();
	void applyForce();
	void levelUp(QueueElement* done) const;
	void updateBuildingQueues(float time);
	void updateQueues();
	void selfAI();
	void addTestEntities() const;

	void tryToAttack(Unit* unit);
	void tryToCollect(Unit* unit);
	void tryToShot(Unit* unit);

	float accumulateTime = 0;
	float maxTimeFrame = 0.05f;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	ColorMode currentColor = ColorMode::BASIC;
	Force force;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	SimulationInfo* simulationInfo;
	Enviroment* enviroment;
	SimulationObjectManager* simObjectManager;
	CreationCommandList* creationCommandList;
	UpgradeCommandList* levelsCommandList;
	CommandList* actionCommandList;
};
