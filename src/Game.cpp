#include "Game.h"

Game* Game::instance = nullptr;

Urho3D::ResourceCache* Game::getCache() const {
	return cache;
}

Urho3D::SharedPtr<Urho3D::Scene> Game::getScene() const {
	return scene;
}

Urho3D::Context* Game::getContext() const {
	return context;
}

Urho3D::UI* Game::getUI() const {
	return ui;
}

Urho3D::Graphics* Game::getGraphics() const {
	return graphics;
}

Urho3D::SharedPtr<Urho3D::Engine> Game::getEngine() const {
	return engine;
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

Game* Game::setCache(Urho3D::ResourceCache* _cache) {
	cache = _cache;
	return this;
}

Game* Game::setContext(Urho3D::Context* _context) {
	context = _context;
	return this;
}

Game* Game::setUI(Urho3D::UI* _ui) {
	ui = _ui;
	return this;
}

Game* Game::setGraphics(Urho3D::Graphics* _graphics) {
	graphics = _graphics;
	return this;
}

Game* Game::setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene) {
	this->scene = scene;
	return this;
}

Game* Game::setEngine(const Urho3D::SharedPtr<Urho3D::Engine>& _engine) {
	engine = _engine;
	return this;
}
