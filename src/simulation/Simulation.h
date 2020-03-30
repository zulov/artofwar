#pragma once

#include "force/Force.h"
#include <vector>
#include <functional>

enum class UnitAction : char;
enum class UnitState : char;
enum class SimColorMode : char;
struct NewGameForm;
class Unit;
class ResourceEntity;
class Building;
class QueueElement;
class Environment;
class SceneSaver;
class SceneLoader;
class CreationCommandList;
class SimulationInfo;
class CreationCommandList;
class SimulationObjectManager;
class UpgradeCommandList;
class CommandList;
class AiManager;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation
{
public:
	explicit Simulation(Environment* enviroment);
	~Simulation();

	SimulationInfo* update(float timeStep);
	void initScene(SceneLoader& loader) const;
	void initScene(NewGameForm* form) const;

	void dispose() const;
	void save(SceneSaver& saver) const;
	void changeCoef(int i, int wheel);
	void changeColorMode(SimColorMode _colorMode);

private:
	void aiPlayers() const;
	void moveUnits(float timeStep) const;
	void moveUnitsAndCheck(float timeStep) const;
	void calculateForces();
	void performStateAction(float timeStep) const;
	void handleTimeInFrame(float timeStep);
	float updateTime(float timeStep);

	void loadEntities(NewGameForm* form) const;
	void loadEntities(SceneLoader& loader) const;
	void countFrame();
	void applyForce() const;
	void levelUp(QueueElement* done, char player) const;
	void updateBuildingQueues(float time) const;
	void updateQueues() const;
	void selfAI();
	void addTestEntities() const;

	void tryToAttack(Unit* unit, float dist, UnitAction order, const std::function<bool(Physical*)>& condition);
	void toAction(Unit* unit, std::vector<Physical*>* list,  UnitAction order, const std::function<bool(Physical*)>& condition);
	void tryToCollect(Unit* unit);

	float accumulateTime = 0;
	float maxTimeFrame = 0.05f;
	int framesPeriod = 20;
	int currentFrameNumber = 0;
	SimColorMode colorScheme;
	Force force;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	SimulationInfo* simulationInfo;
	Environment* enviroment;
	SimulationObjectManager* simObjectManager;

	AiManager* aiManager;
};

