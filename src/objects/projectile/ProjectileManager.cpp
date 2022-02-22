#include "ProjectileManager.h"

#include "ProjectileWithNode.h"
#include "simulation/SimGlobals.h"
#include "objects/NodeUtils.h"
#include "utils/DeleteUtils.h"

ProjectileManager* ProjectileManager::instance = nullptr;

constexpr int MAX_PROJECTILE_NODES = 100;

void ProjectileManager::update(float timeStep) {
	for (const auto projectile : instance->projectiles) {
		projectile->update(timeStep);
		//TODO perf free pool
	}
}

void ProjectileManager::shoot(Physical* shooter, Physical* aim, float speed, char player) {
	findNext()->init(shooter, aim, speed, player);
}

ProjectileBase* ProjectileManager::findNext() {
	for (const auto projectile : instance->projectiles) {
		if (!projectile->isActive()) {
			return projectile;
		}
	}

	Urho3D::Node* node = nullptr;
	ProjectileBase* projectile;
	if (instance->projectiles.size() < MAX_PROJECTILE_NODES && !SIM_GLOBALS.HEADLESS) {
		projectile = new ProjectileWithNode(createNode("Objects/projectiles/arrow.xml"));
	} else {
		projectile = new ProjectileBase();
	}

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
