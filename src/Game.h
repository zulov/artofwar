#pragma once
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Scene/Scene.h>


namespace Urho3D {
	class Console;
	class Graphics;
	class Context;
	class ResourceCache;
	class UI;
}

class CommandList;
class CreationCommandList;
class Environment;
class DatabaseCache;
class CameraManager;
class PlayersManager;
class QueueManager;
class ColorPeletteRepo;
class FormationManager;

class Game
{
public:
	static void init();
	static void dispose();
	~Game();
	static Game* setCache(Urho3D::ResourceCache* _cache);
	static Game* setContext(Urho3D::Context* _context);
	static Game* setUI(Urho3D::UI* _ui);
	static Game* setGraphics(Urho3D::Graphics* _graphics);
	static Game* setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene);
	static Game* setEngine(const Urho3D::SharedPtr<Urho3D::Engine>& _engine);
	static Game* setConsole(Urho3D::Console* _console);
	static Game* setLocalization(Urho3D::Localization* _localization);
	static Game* setCameraManager(CameraManager* cameraManager);
	static Game* setActionCommmandList(CommandList* _actionCommmandList);
	static Game* setCreationCommandList(CreationCommandList* _simCommandList);
	static Game* setDatabaseCache(DatabaseCache* _databaseCache);
	static Game* setPlayersManager(PlayersManager* _playersManager);
	static Game* setEnvironment(Environment* _environment);
	static Game* setQueueManager(QueueManager* _queueManager);
	static Game* setFormationManager(FormationManager* _formationManager);
	static Game* setColorPeletteRepo(ColorPeletteRepo* _colorPeletteRepo);

	static Urho3D::ResourceCache* getCache() { return instance->cache; }
	static Urho3D::SharedPtr<Urho3D::Scene> getScene() { return instance->scene; }
	static Urho3D::Context* getContext() { return instance->context; }
	static Urho3D::UI* getUI() { return instance->ui; }
	static Urho3D::Graphics* getGraphics() { return instance->graphics; }
	static Urho3D::SharedPtr<Urho3D::Engine> getEngine() { return instance->engine; }
	static Urho3D::Console* getConsole() { return instance->console; }
	static Urho3D::Localization* getLocalization() { return instance->localization; }
	static CommandList* getActionList() { return instance->actionCommandList; }
	static QueueManager* getQueueManager() { return instance->queue; }
	static CreationCommandList* getCreationList() { return instance->creationCommandList; }
	static DatabaseCache* getDatabaseCache() { return instance->databaseCache; }
	static PlayersManager* getPlayersMan() { return instance->playersManager; }
	static Environment* getEnvironment() { return instance->environment; }
	static FormationManager* getFormationManager() { return instance->formationManager; }
	static ColorPeletteRepo* getColorPeletteRepo() { return instance->colorPeletteRepo; }
	static CameraManager* getCameraManager() { return instance->cameraManager; }

private:
	Game();
	static Game* instance;

	Urho3D::ResourceCache* cache;
	Urho3D::SharedPtr<Urho3D::Scene> scene;
	Urho3D::Context* context;
	Urho3D::UI* ui;
	Urho3D::Graphics* graphics;
	Urho3D::SharedPtr<Urho3D::Engine> engine;
	Urho3D::Console* console;
	Urho3D::Localization* localization;

	CameraManager* cameraManager;
	CommandList* actionCommandList;
	CreationCommandList* creationCommandList;
	DatabaseCache* databaseCache;
	PlayersManager* playersManager;
	Environment* environment;
	QueueManager* queue;
	FormationManager* formationManager;
	ColorPeletteRepo* colorPeletteRepo;
};
