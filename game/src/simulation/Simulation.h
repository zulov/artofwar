#pragma once

#include <functional>
#include <vector>
#include "force/Force.h"

#include "database/db_struct.h"

struct dbload_container;
struct db_map;
struct FrameInfo;
enum class UnitAction : unsigned char;
enum class UnitState : unsigned char;
enum class SimColorMode : unsigned char;
struct NewGameForm;
class Unit;
class ResourceEntity;
class Building;
class QueueElement;
class Environment;
class SceneSaver;
class SceneLoader;
class CreationCommandList;
class SimulationObjectManager;
class UpgradeCommandList;
class CommandList;
class WorldAgeController;

namespace Urho3D {
	class Node;
	class Scene;
} // namespace Urho3D

class Simulation {
public:
	Simulation(Environment* environment, const db_map* map);
	~Simulation();
	void clearNodesWithoutDelete() const;

	void updateInfluenceMaps(bool force) const;

	FrameInfo* update(float timeStep);
	void initScene(SceneLoader& loader) const;
	void initScene(NewGameForm* form) const;
	void restorePendingCommands(SceneLoader& loader) const;

	void changeCoef(int i, int wheel);
	void changeColorMode(SimColorMode _colorMode);
	const std::vector<Unit*>* getUnits() const { return units; }
	const std::vector<Building*>* getBuildings() const { return buildings; }
	const std::vector<ResourceEntity*>* getResources() const { return resources; }
	const WorldAgeController* getWorldAgeController() const { return worldAgeController; }

private:
	void aiPlayers() const;
	void calculateForces();
	void moveUnitsAndCheck();
	void colorUnits();
	void performStateAction() const;
	void executeStateTransition() const;

	void loadEntities(NewGameForm* form) const;
	void loadEntities(dbload_container* data) const;
	void restoreRuntimeState(dbload_container* data) const;
	void applyForce() const;
	void levelUp(QueueElement* done, unsigned char playerId) const;
	void updateBuildingQueues() const;
	void updateQueues() const;
	std::function<bool(Physical*)> ifAttack(db_unit* dbUnit) const;
	void objectAI() const;
	void addTestEntities() const;

	SimColorMode colorScheme;
	bool colorSchemeChanged = true;
	Force force;

	const std::vector<Unit*>* units;
	const std::vector<Building*>* buildings;
	const std::vector<ResourceEntity*>* resources;

	Environment* env;
	SimulationObjectManager* simObjectManager;
	WorldAgeController* worldAgeController;
};
