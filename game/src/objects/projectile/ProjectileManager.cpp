#include "ProjectileManager.h"

#include <unordered_map>
#include "ProjectileWithNode.h"
#include "objects/NodeUtils.h"
#include "simulation/SimGlobals.h"
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
	for (auto it = instance->projectiles.rbegin(); it != instance->projectiles.rend(); ++it) {
		(*it)->reset();
		instance->freeList.push_back(*it);
	}
}

void ProjectileManager::clearNodesWithoutDelete() {
	for (auto* projectile : instance->projectiles) {
		projectile->clearNodeWithoutDelete();
	}
}

void ProjectileManager::returnToPool(ProjectileBase* projectile) { instance->freeList.push_back(projectile); }

std::vector<ProjectileSaveData> ProjectileManager::saveState() {
	std::vector<ProjectileSaveData> state;
	for (const auto projectile : instance->projectiles) {
		if (projectile->isActive()) {
			const auto saved = projectile->saveState();
			if (saved.aimUid != 0) {
				state.push_back(saved);
			}
		}
	}
	return state;
}

void ProjectileManager::loadState(const std::vector<ProjectileSaveData>& state,
								  const std::unordered_map<unsigned, Physical*>& byUid) {
	for (const auto& saved : state) {
		const auto it = byUid.find(saved.aimUid);
		if (it != byUid.end()) {
			findNext()->loadState(saved, it->second);
		}
	}
}

ProjectileManager::~ProjectileManager() { clear_vector(projectiles); }
