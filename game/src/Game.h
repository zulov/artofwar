#pragma once
#include <Urho3D/Scene/Scene.h>

namespace Urho3D {
	class Console;
	class Graphics;
	class Context;
	class ResourceCache;
	class UI;
	class Log;
	class Localization;
	class Engine;
}

class Environment;
class DatabaseCache;
class CameraManager;
class PlayersManager;
class ColorPaletteRepo;
class FormationManager;
class ActionCenter;
class AiInputProvider;

class Game {
public:
	static void init();
	static void dispose();
	~Game() = default;

	static Game* setCache(Urho3D::ResourceCache* _cache);
	static Game* setContext(Urho3D::Context* _context);
	static Game* setUI(Urho3D::UI* _ui);
	static Game* setGraphics(Urho3D::Graphics* _graphics);
	static Game* setScene(Urho3D::Scene* scene);
	static Game* setConsole(Urho3D::Console* _console);
	static Game* setLocalization(Urho3D::Localization* _localization);
	static Game* setCameraManager(CameraManager* cameraManager);
	static Game* setDatabaseCache(DatabaseCache* _databaseCache);
	static Game* setPlayersManager(PlayersManager* _playersManager);
	static Game* setEnvironment(Environment* _environment);
	static Game* setFormationManager(FormationManager* _formationManager);
	static Game* setColorPaletteRepo(ColorPaletteRepo* _colorPaletteRepo);
	static Game* setLog(Urho3D::Log* _log);
	static Game* setAiInputProvider(AiInputProvider* _aiInputProvider);
	static Game* setActionCenter(ActionCenter* _actionCenter);
	static void addTime(float time);

	static Urho3D::Localization* getLocalization() { return instance->localization; }
	static Urho3D::Scene* getScene() { return instance->scene; }
	static Urho3D::Graphics* getGraphics() { return instance->graphics; }
	static Urho3D::ResourceCache* getCache() { return instance->cache; }
	static Urho3D::Context* getContext() { return instance->context; }
	static Urho3D::Console* getConsole() { return instance->console; }
	static Urho3D::Log* getLog() { return instance->log; }
	static Urho3D::UI* getUI() { return instance->ui; }

	static FormationManager* getFormationManager() { return instance->formationManager; }
	static ColorPaletteRepo* getColorPaletteRepo() { return instance->colorPaletteRepo; }
	static DatabaseCache* getDatabase() { return instance->databaseCache; }
	static CameraManager* getCameraManager() { return instance->cameraManager; }
	static PlayersManager* getPlayersMan() { return instance->playersManager; }
	static Environment* getEnvironment() { return instance->environment; }
	static AiInputProvider* getAiInputProvider() { return instance->aiInputProvider; }
	static ActionCenter* getActionCenter() { return instance->actionCenter; }

	static float getTime() { return instance->accumTime; }

private:
	Game();
	static Game* instance;

	Urho3D::Scene* scene;
	Urho3D::Localization* localization;
	Urho3D::ResourceCache* cache;
	Urho3D::Graphics* graphics;
	Urho3D::Console* console;
	Urho3D::Context* context;
	Urho3D::Log* log;
	Urho3D::UI* ui;

	FormationManager* formationManager;
	ColorPaletteRepo* colorPaletteRepo;
	PlayersManager* playersManager;
	CameraManager* cameraManager;
	DatabaseCache* databaseCache;
	Environment* environment;

	AiInputProvider* aiInputProvider;
	ActionCenter* actionCenter;

	float accumTime = 0;
};
