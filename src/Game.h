#pragma once
#include "simulation/formation/FormationManager.h"
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

class ActionCommandList;
class CommandList;

class CreationCommandList;
class Enviroment;
class DatabaseCache;
class CameraManager;
class PlayersManager;
class QueueManager;
class ColorPeletteRepo;

class Game
{
public:
	static Game* get();
	static void init();
	static void dispose();
	~Game();
	Game* setCache(ResourceCache* _cache);
	Game* setContext(Context* _context);
	Game* setUI(UI* _ui);
	Game* setGraphics(Graphics* _graphics);
	Game* setScene(const SharedPtr<Scene>& scene);
	Game* setEngine(const SharedPtr<Engine>& _engine);
	Game* setConsole(Console* _console);
	Game* setLocalization(Localization* _localization);
	Game* setCameraManager(CameraManager* cameraManager);
	Game* setActionCommmandList(ActionCommandList* _actionCommmandList);
	Game* setCreationCommandList(CreationCommandList* _simCommandList);
	Game* setDatabaseCache(DatabaseCache* _databaseCache);
	Game* setPlayersManager(PlayersManager* _playersManager);
	Game* setEnviroment(Enviroment* _enviroment);
	Game* setQueueManager(QueueManager* _queueManager);
	Game* setFormationManager(FormationManager* _formationManager);
	Game* setColorPeletteRepo(ColorPeletteRepo* _colorPeletteRepo);


	ResourceCache* getCache() const;
	SharedPtr<Scene> getScene() const;
	Context* getContext() const;
	UI* getUI() const;
	Graphics* getGraphics() const;
	SharedPtr<Engine> getEngine() const;
	Console* getConsole() const;
	Localization* getLocalization() const;

	QueueManager* getQueueManager() const;
	CameraManager* getCameraManager() const;
	ActionCommandList* getActionCommandList() const;
	CreationCommandList* getCreationCommandList() const;
	DatabaseCache* getDatabaseCache() const;
	PlayersManager* getPlayersManager() const;
	Enviroment* getEnviroment() const;
	FormationManager* getFormationManager() const;
	ColorPeletteRepo* getColorPeletteRepo() const;

private:
	Game();
	static Game* instance;

	ResourceCache* cache;
	SharedPtr<Scene> scene;
	Context* context;
	UI* ui;
	Graphics* graphics;
	SharedPtr<Engine> engine;
	Console* console;
	Localization* localization;

	CameraManager* cameraManager;
	ActionCommandList* actionCommandList;
	CreationCommandList* creationCommandList;
	DatabaseCache* databaseCache;
	PlayersManager* playersManager;
	Enviroment* enviroment;
	QueueManager* queue;
	FormationManager* formationManager;
	ColorPeletteRepo* colorPeletteRepo;
};
