#include "Game.h"

Game* Game::instance = nullptr;

Game::Game() = default;

void Game::init() {
	if (instance == nullptr) {
		instance = new Game();
	}
}

void Game::dispose() {
	delete instance;
}

Game::~Game() = default;

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

Game* Game::setCameraManager(CameraManager* cameraManager) {
	this->cameraManager = cameraManager;
	return this;
}

Game* Game::setActionCommmandList(CommandList* _actionCommmandList) {
	actionCommandList = _actionCommmandList;
	return this;
}

Game* Game::setConsole(Urho3D::Console* _console) {
	console = _console;
	return this;
}

Game* Game::setLocalization(Urho3D::Localization* _localization) {
	localization = _localization;
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

Game* Game::setQueueManager(QueueManager* _queueManager) {
	queue = _queueManager;
	return this;
}

Game* Game::setFormationManager(FormationManager* _formationManager) {
	formationManager = _formationManager;
	return this;
}

Game* Game::setColorPeletteRepo(ColorPeletteRepo* _colorPeletteRepo) {
	colorPeletteRepo = _colorPeletteRepo;
	return this;
}
