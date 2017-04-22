#pragma once
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Engine/Engine.h>
#include "CameraManager.h"
#include "CommandList.h"

class Mediator;

class Game
{
public:
	static Game* getInstance();
	~Game();
	Game* setCache(Urho3D::ResourceCache* _cache);
	Game* setContext(Urho3D::Context* _context);
	Game* setUI(Urho3D::UI* _ui);
	Game* setGraphics(Urho3D::Graphics* _graphics);
	Game* setScene(const Urho3D::SharedPtr<Urho3D::Scene>& scene);
	Game* setEngine(const Urho3D::SharedPtr<Urho3D::Engine>& _engine);
	Game* setCameraManager(CameraManager* cameraManager);
	Game* setCommmandList(CommandList* _commmandList);
	Game* setConsole(Console* _console);
	Game* setMediator(Mediator* _mediator);

	CameraManager* getCameraManager() const;
	Urho3D::ResourceCache* getCache() const;
	Urho3D::SharedPtr<Urho3D::Scene> getScene() const;
	Urho3D::Context* getContext() const;
	Urho3D::UI* getUI() const;
	Urho3D::Graphics* getGraphics() const;
	Urho3D::SharedPtr<Urho3D::Engine> getEngine() const;
	CommandList* getCommandList() const;
	Console* getConsole() const;
	Mediator* getMediator() const;
private:
	Game();
	static Game* instance;
	Urho3D::ResourceCache* cache;
	Urho3D::SharedPtr<Urho3D::Scene> scene;
	Urho3D::Context* context;
	Urho3D::UI* ui;
	Urho3D::Graphics* graphics;
	Urho3D::SharedPtr<Urho3D::Engine> engine;
	CameraManager * cameraManager;
	CommandList * commmandList;
	Console* console;
	Mediator * mediator;

};
