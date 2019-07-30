#pragma once

#include "force/Force.h"
#include <vector>
#include <functional>

enum class UnitOrder : char;
enum class UnitState : char;
enum class ColorMode : char;
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
	Simulation(Environment* enviroment, CreationCommandList* creationCommandList);
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
	void aiPlayers();
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
	void levelUp(QueueElement* done) const;
	void updateBuildingQueues(float time) const;
	void updateQueues() const;
	void selfAI();
	void addTestEntities() const;

	void tryToAttack(Unit* unit, float dist, UnitOrder order, const std::function<bool(Physical*)>& condition);
	void toAction(Unit* unit, std::vector<Physical*>* list,  UnitOrder order, const std::function<bool(Physical*)>& condition);
	void tryToCollect(Unit* unit);

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
	AiManager* aiManager;
};

