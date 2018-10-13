#pragma once

#include "force/Force.h"
#include <vector>

enum class UnitState : char;
class Unit;
class ResourceEntity;
class Building;
class QueueElement;
enum class ColorMode : char;
struct NewGameForm;
class Environment;
class SceneSaver;
class SceneLoader;
class CreationCommandList;
class SimulationInfo;
class CreationCommandList;
class SimulationObjectManager;
class UpgradeCommandList;
class CommandList;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	Simulation(Environment* _enviroment, CreationCommandList* _creationCommandList);
	~Simulation();
	SimulationInfo* update(float timeStep);
	void executeLists() const;
	void initScene(SceneLoader& loader) const;
	void initScene(NewGameForm* form) const;

	void dispose() const;
	void save(SceneSaver& saver) const;
	void changeCoef(int i, int wheel);
	void changeColorMode(ColorMode _colorMode);

private:
	void moveUnits(float timeStep) const;
	void moveUnitsAndCheck(float timeStep) const;
	void calculateForces();
	void performStateAction(float timeStep) const;
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
	void toAction(Unit* unit, std::vector<Physical*>* list, UnitState state);
	void tryToCollect(Unit* unit);
	void tryToShot(Unit* unit);

	float accumulateTime = 0;
	float maxTimeFrame = 0.05f;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	ColorMode colorScheme;
	Force force;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	SimulationInfo* simulationInfo;
	Environment* enviroment;
	SimulationObjectManager* simObjectManager;
	CreationCommandList* creationCommandList;
	UpgradeCommandList* levelsCommandList;
	CommandList* actionCommandList;
};
