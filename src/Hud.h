#pragma once
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Drawable.h>
#include "CameraManager.h"
#include "Benchmark.h"


using namespace Urho3D;

class Hud {
public:
	Hud(Context* context, UI* _ui, ResourceCache* _cache, Graphics* _graphics);
	~Hud();
	void createStaticHud(String msg);
	void createLogo();
	void createDebugHud(SharedPtr<Engine> engine);
	void createConsole(SharedPtr<Engine> engine);
	void updateHud(Benchmark * benchmark, CameraManager *cameraManager);

private:
	Window *window;
	UI* ui;
	ResourceCache* cache;
	Text* fpsText;
};

