#include "ProjectileManager.h"

ProjectileManager* ProjectileManager::instance = nullptr;



void ProjectileManager::init() {
	if (instance == nullptr) {
		instance = new ProjectileManager();
	}
}

void ProjectileManager::dispose() {
	delete instance;
	instance = nullptr;
}