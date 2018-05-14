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
class Enviroment;
class DatabaseCache;
class CameraManager;
class PlayersManager;
class QueueManager;
class ColorPeletteRepo;
class FormationManager;

class Game
{
public:
	static Game* get() { return instance; }
	static void init();
	static void dispose();
	~Game();
	Game* setCache(Urho3D::ResourceCache* _cache);
	Game* setContext(Urho3D::Context* _context);
	Game* setUI(Urho3D::UI* _ui);
	Game* setGraphics(Urho3D::Graphics* _graphics);
	Game* setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene);
	Game* setEngine(const Urho3D::SharedPtr<Urho3D::Engine>& _engine);
	Game* setConsole(Urho3D::Console* _console);
	Game* setLocalization(Urho3D::Localization* _localization);
	Game* setCameraManager(CameraManager* cameraManager);
	Game* setActionCommmandList(CommandList* _actionCommmandList);
	Game* setCreationCommandList(CreationCommandList* _simCommandList);
	Game* setDatabaseCache(DatabaseCache* _databaseCache);
	Game* setPlayersManager(PlayersManager* _playersManager);
	Game* setEnviroment(Enviroment* _enviroment);
	Game* setQueueManager(QueueManager* _queueManager);
	Game* setFormationManager(FormationManager* _formationManager);
	Game* setColorPeletteRepo(ColorPeletteRepo* _colorPeletteRepo);

	Urho3D::ResourceCache* getCache() const { return cache; }
	Urho3D::SharedPtr<Urho3D::Scene> getScene() const { return scene; }
	Urho3D::Context* getContext() const { return context; }
	Urho3D::UI* getUI() const { return ui; }
	Urho3D::Graphics* getGraphics() const { return graphics; }
	Urho3D::SharedPtr<Urho3D::Engine> getEngine() const { return engine; }
	CommandList* getActionCommandList() const { return actionCommandList; }
	Urho3D::Console* getConsole() const { return console; }
	Urho3D::Localization* getLocalization() const { return localization; }
	QueueManager* getQueueManager() const { return queue; }
	CreationCommandList* getCreationCommandList() const { return creationCommandList; }
	DatabaseCache* getDatabaseCache() const { return databaseCache; }
	PlayersManager* getPlayersManager() const { return playersManager; }
	Enviroment* getEnviroment() const { return enviroment; }
	FormationManager* getFormationManager() const { return formationManager; }
	ColorPeletteRepo* getColorPeletteRepo() const { return colorPeletteRepo; }
	CameraManager* getCameraManager() const { return cameraManager; }

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
	Enviroment* enviroment;
	QueueManager* queue;
	FormationManager* formationManager;
	ColorPeletteRepo* colorPeletteRepo;
};
