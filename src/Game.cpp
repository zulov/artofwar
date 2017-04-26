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

CommandList* Game::getCommandList() const {
	return commmandList;
}

Console* Game::getConsole() const {
	return console;
}

Mediator* Game::getMediator() const {
	return mediator;
}

BuildList* Game::getBuildList() const {
	return buildList;
}

Game::Game() {
}


Game* Game::get() {
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

CameraManager* Game::getCameraManager() const {
	return cameraManager;
}

Game* Game::setCameraManager(CameraManager* cameraManager) {
	this->cameraManager = cameraManager;
	return this;
}

Game* Game::setCommmandList(CommandList* _commmandList) {
	commmandList = _commmandList;
	return this;
}

Game* Game::setConsole(Console* _console) {
	console = _console;
	return this;
}

Game* Game::setMediator(Mediator* _mediator) {
	mediator = _mediator;
	return this;
}

Game* Game::setBuildList(BuildList* _buildList) {
	buildList = _buildList;
	return this;
}
