#include "Game.h"
#include "player/ai/ActionCenter.h"

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

void Game::disposeActionCenter() {
	delete instance->actionCenter;
	instance->actionCenter = nullptr;
}

Game* Game::setCache(Urho3D::ResourceCache* _cache) {
	instance->cache = _cache;
	return instance;
}

Game* Game::setContext(Urho3D::Context* _context) {
	instance->context = _context;
	return instance;
}

Game* Game::setUI(Urho3D::UI* _ui) {
	instance->ui = _ui;
	return instance;
}

Game* Game::setGraphics(Urho3D::Graphics* _graphics) {
	instance->graphics = _graphics;
	return instance;
}

Game* Game::setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene) {
	instance->scene = scene;
	return instance;
}

Game* Game::setEngine(const Urho3D::SharedPtr<Urho3D::Engine>& _engine) {
	instance->engine = _engine;
	return instance;
}

Game* Game::setCameraManager(CameraManager* cameraManager) {
	instance->cameraManager = cameraManager;
	return instance;
}

Game* Game::setConsole(Urho3D::Console* _console) {
	instance->console = _console;
	return instance;
}

Game* Game::setLocalization(Urho3D::Localization* _localization) {
	instance->localization = _localization;
	return instance;
}

Game* Game::setDatabaseCache(DatabaseCache* _databaseCache) {
	instance->databaseCache = _databaseCache;
	return instance;
}

Game* Game::setPlayersManager(PlayersManager* _playersManager) {
	instance->playersManager = _playersManager;
	return instance;
}

Game* Game::setEnvironment(Environment* _environment) {
	instance->environment = _environment;
	return instance;
}

Game* Game::setFormationManager(FormationManager* _formationManager) {
	instance->formationManager = _formationManager;
	return instance;
}

Game* Game::setColorPaletteRepo(ColorPaletteRepo* _colorPaletteRepo) {
	instance->colorPaletteRepo = _colorPaletteRepo;
	return instance;
}

Game* Game::setLog(Urho3D::Log* _log) {
	instance->log = _log;
	return instance;
}

Game* Game::setStats(Stats* _stats) {
	instance->stats = _stats;
	return instance;
}

Game* Game::setActionCenter(ActionCenter* _actionCenter) {
	instance->actionCenter = _actionCenter;
	return instance;
}

void Game::addTime(float time) {
	instance->accumTime += time;
}
