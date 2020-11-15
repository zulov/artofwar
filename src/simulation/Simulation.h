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

class Simulation {
public:
	explicit Simulation(Environment* enviroment);
	~Simulation();
	void updateInfluenceMaps();

	SimulationInfo* update(float timeStep);
	void initScene(SceneLoader& loader) const;
	void initScene(NewGameForm* form) const;

	void save(SceneSaver& saver) const;
	void changeCoef(int i, int wheel);
	void changeColorMode(SimColorMode _colorMode);

private:
	void aiPlayers() const;
	void moveUnitsAndCheck(float timeStep);
	void calculateForces();
	void performStateAction(float timeStep) const;
	float updateTime(float timeStep) const;

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
	void toAction(Unit* unit, std::vector<Physical*>* list, UnitAction order,
	              const std::function<bool(Physical*)>& condition);
	void tryToCollect(Unit* unit);

	float accumulateTime = 0;
	unsigned int secondsElapsed = 0;
	unsigned char currentFrameNumber = 0;
	SimColorMode colorScheme;
	bool colorSchemeChanged = true;
	Force force;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	SimulationInfo* simulationInfo;
	Environment* enviroment;
	SimulationObjectManager* simObjectManager;

	AiManager* aiManager;
};
