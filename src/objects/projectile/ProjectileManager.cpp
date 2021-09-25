#include "ProjectileManager.h"

#include "ProjectileData.h"
#include "simulation/SimGlobals.h"
#include "objects/NodeUtils.h"
#include "simulation/env/Environment.h"
#include "utils/DeleteUtils.h"

ProjectileManager* ProjectileManager::instance = nullptr;

constexpr int MAX_PROJECTILE_NODES = 100;

void ProjectileManager::update(float timeStep) {
	for (const auto projectile : instance->projectiles) {
		projectile->update(timeStep);
		//TODO perf set lowest free
	}
}

void ProjectileManager::shoot(const Urho3D::Vector3& start, Physical* aim, float speed, char player, float attackVal) {
	ProjectileData* data = findNext();

	data->init(start, aim, speed, player, attackVal);
}

ProjectileData* ProjectileManager::findNext() {
	for (const auto projectile : instance->projectiles) {
		if (!projectile->isActive()) {
			return projectile;
		}
	}

	Urho3D::Node* node = nullptr;
	if (instance->projectiles.size() < MAX_PROJECTILE_NODES && !SIM_GLOBALS.HEADLESS) {
		node = createNode("Objects/projectiles/arrow.xml");
	}
	const auto projectile = new ProjectileData(node);
	instance->projectiles.push_back(projectile);

	return projectile;
}

void ProjectileManager::init() {
	if (instance == nullptr) {
		instance = new ProjectileManager();
	}
}

void ProjectileManager::dispose() {
	delete instance;
	instance = nullptr;
}


ProjectileManager::~ProjectileManager() {
	clear_vector(projectiles);
}
