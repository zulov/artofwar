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
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

class Hud {
public:
	Hud(Context* context, UI* _ui, ResourceCache* _cache);
	~Hud();
	void createStaticHud(String msg);
private:
	Window *window;
	UI* ui;
	ResourceCache* cache;
};

