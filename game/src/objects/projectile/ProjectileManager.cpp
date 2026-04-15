#include "ProjectileManager.h"

#include "ProjectileWithNode.h"
#include "simulation/SimGlobals.h"
#include "objects/NodeUtils.h"
#include "utils/DeleteUtils.h"

ProjectileManager* ProjectileManager::instance = nullptr;

constexpr int MAX_PROJECTILE_NODES = 100;

void ProjectileManager::update(float timeStep) {
	for (const auto projectile : instance->projectiles) {
		if (projectile->isActive()) {
			if (projectile->update(timeStep)) {
				instance->freeList.push_back(projectile);
			}
		}
	}
}

void ProjectileManager::shoot(Physical* shooter, Physical* aim, float speed, char player) {
	findNext()->init(shooter, aim, speed, player);
}

ProjectileBase* ProjectileManager::findNext() {
	if (!instance->freeList.empty()) {
		auto* projectile = instance->freeList.back();
		instance->freeList.pop_back();
		return projectile;
	}

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

void ProjectileManager::reset() {
	instance->freeList.clear();
	for (auto projectile : instance->projectiles) {
		projectile->reset();
		instance->freeList.push_back(projectile);
	}
}

void ProjectileManager::returnToPool(ProjectileBase* projectile) {
	instance->freeList.push_back(projectile);
}


ProjectileManager::~ProjectileManager() {
	clear_vector(projectiles);
}
