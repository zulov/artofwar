#pragma once

#include "force/Force.h"
#include <vector>
#include <functional>

#include "PerFrameAction.h"
#include "database/db_strcut.h"

struct FrameInfo;
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

namespace Urho3D {
	class Node;
	class Scene;
}

class Simulation {
public:
	explicit Simulation(Environment* enviroment, unsigned currentResourceUid);
	~Simulation();
	void clearNodesWithoutDelete() const;

	void updateInfluenceMaps(bool force) const;
	bool canUpdate(PerFrameAction type, bool force) const;

	FrameInfo* update(float timeStep);
	void initScene(SceneLoader& loader) const;
	void initScene(NewGameForm* form) const;

	void save(SceneSaver& saver) const;
	void changeCoef(int i, int wheel);
	void changeColorMode(SimColorMode _colorMode);

private:
	void aiPlayers() const;
	void calculateForces();
    void moveUnitsAndCheck();
    void colorUnits();
    void performStateAction() const;
	void executeStateTransition() const;
	float updateTime(float timeStep) const;

	void loadEntities(NewGameForm* form) const;
	void loadEntities(dbload_container* data) const;
	void countFrame();
	void applyForce() const;
	void levelUp(QueueElement* done, char player) const;
	void updateBuildingQueues() const;
	void updateQueues() const;
	std::function<bool(Physical*)> ifAttack(db_unit* dbUnit) const;
	void objectAI() const;
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

	FrameInfo* frameInfo;
	Environment* env;
	SimulationObjectManager* simObjectManager;
};
