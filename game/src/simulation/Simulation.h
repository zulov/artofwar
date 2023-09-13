#pragma once

#include "force/Force.h"
#include <vector>
#include <functional>

#include "database/db_strcut.h"

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
class CreationCommandList;
class SimulationObjectManager;
class UpgradeCommandList;
class CommandList;
class AiManager;
class SimInfo;

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation {
public:
	explicit Simulation(Environment* enviroment);
	~Simulation();
	void clearNodesWithoutDelete();
	
	void updateInfluenceMaps() const;
	void forceUpdateInfluenceMaps() const;

	SimInfo* update(float timeStep);
	void initScene(SceneLoader& loader) const;
	void initScene(NewGameForm* form) const;

	void save(SceneSaver& saver) const;
	void changeCoef(int i, int wheel);
	void changeColorMode(SimColorMode _colorMode);

private:
	void aiPlayers() const;
	void calculateForces();
	void moveUnitsAndCheck(float timeStep);
	void performStateAction(float timeStep) const;
	void executeStateTransition() const;
	float updateTime(float timeStep) const;

	void loadEntities(NewGameForm* form) const;
	void loadEntities(SceneLoader& loader) const;
	void countFrame();
	void applyForce() const;
	void levelUp(QueueElement* done, char player) const;
	void updateBuildingQueues() const;
	void updateQueues() const;
	std::function<bool(Physical*)> getCondition(db_unit* dbUnit) const;
	void selfAI() const;
	void addTestEntities() const;

	float accumulateTime = 0;
	unsigned int secondsElapsed = 0;
	unsigned char currentFrame = 0;
	SimColorMode colorScheme;
	bool colorSchemeChanged = true;
	Force force;

	std::vector<Unit*>* units;
	std::vector<Building*>* buildings;
	std::vector<ResourceEntity*>* resources;

	SimInfo* simInfo;
	Environment* enviroment;
	SimulationObjectManager* simObjectManager;

	AiManager* aiManager;
};
