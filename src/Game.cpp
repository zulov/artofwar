#include "Game.h"
#include "camera/CameraManager.h"

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

ActionCommandList* Game::getActionCommandList() const {
	return actionCommandList;
}

Urho3D::Console* Game::getConsole() const {
	return console;
}

CreationCommandList* Game::getCreationCommandList() const {
	return creationCommandList;
}

DatabaseCache* Game::getDatabaseCache() const {
	return databaseCache;
}

PlayersManager* Game::getPlayersManager() const {
	return playersManager;
}

Enviroment* Game::getEnviroment() const {
	return enviroment;
}

Game::Game() {
}


Game* Game::get() {
	return instance;
}

void Game::init() {
	if (instance == nullptr) {
		instance = new Game();
	}
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

Game* Game::setActionCommmandList(ActionCommandList* _actionCommmandList) {
	actionCommandList = _actionCommmandList;
	return this;
}

Game* Game::setConsole(Urho3D::Console* _console) {
	console = _console;
	return this;
}

Game* Game::setCreationCommandList(CreationCommandList* _simCommandList) {
	creationCommandList = _simCommandList;
	return this;
}

Game* Game::setDatabaseCache(DatabaseCache* _databaseCache) {
	databaseCache = _databaseCache;
	return this;
}

Game* Game::setPlayersManager(PlayersManager* _playersManager) {
	playersManager = _playersManager;
	return this;
}

Game* Game::setEnviroment(Enviroment* _enviroment) {
	enviroment = _enviroment;
	return this;
}
