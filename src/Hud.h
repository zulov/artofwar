#pragma once
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/DropDownList.h>
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
#include <array>
#include "HudElement.h"
#include "db_strcut.h"
#include "ControlsState.h"

using namespace Urho3D;

class Hud {
public:
	Hud();
	~Hud();
	void createStaticHud(String msg);
	void createLogo();
	void createDebugHud();
	void createConsole();
	void updateHud(Benchmark * benchmark, CameraManager *cameraManager);
	std::vector<HudElement*> *getButtonsToSubscribe();
	std::vector<HudElement*> *getListsToSubscribe();
	void updateState(ControlsState state);
private:
	void createMenu();
	void createBuild();
	void createUnits();
	void createTop();
	Window *menuWindow;
	Window *miniMapWindow;
	Window *topWindow;
	Window *buildWindow;
	Window *unitsWindow;

	Text* fpsText;
	std::vector<HudElement*> *buttons;
	std::vector<HudElement*> *lists;

	XMLFile* style;
	db_hud_size* hudSize;
	Font* font;
	template<std::size_t SIZE>
	void createBuildingIcons(std::array<BuildingType, SIZE> buildings);
	template<std::size_t SIZE>
	void createUnitIcons(std::array<UnitType, SIZE> units);
	template<std::size_t SIZE>
	void populateList(Font* font, DropDownList* dropDownList, std::array<String, SIZE> elements);

	void initDropDownList(DropDownList* dropDownList);
};

