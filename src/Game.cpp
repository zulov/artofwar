#include "Game.h"

Game* Game::instance = nullptr;

void Game::setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene) {
	this->scene = scene;
}

Urho3D::ResourceCache* Game::getCache() const {
	return cache;
}

Urho3D::SharedPtr<Urho3D::Scene> Game::getScene() const {
	return scene;
}

Game::Game() {
}


Game* Game::getInstance() {
	if (instance == nullptr) {
		instance = new Game();
	}
	return instance;
}

Game::~Game() {
}

void Game::setCache(Urho3D::ResourceCache* _cache) {
	this->cache = _cache;
}
