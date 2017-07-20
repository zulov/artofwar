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
#include <Urho3D/UI/ListView.h>
#include "CameraManager.h"
#include "Benchmark.h"
#include <array>
#include "HudElement.h"
#include "db_strcut.h"
#include "ControlsState.h"
#include "SelectedInfo.h"
#include "SelectedHudPanel.h"

using namespace Urho3D;

class Hud 
{
public:
	void replaceVariables(XMLFile* xmlFile, int hudSizeId);
	Hud();
	~Hud();
	void createDebugHud();
	void createConsole();
	void createMyDebugHud();
	void updateHud(Benchmark* benchmark, CameraManager* cameraManager);
	std::vector<HudElement*>* getButtonsToSubscribe();
	std::vector<HudElement*>* getListsToSubscribe();
	std::vector<Window*>* getWindows();
	void updateState(ControlsState state);
	void updateSelected(SelectedInfo* selectedInfo);
private:
	Window * createWindow(const String& styleName);

	void createMenu();
	void createBuild();
	void createUnits();
	void createTop();
	void createSelectedInfo();
	void createMiniMap();
	Window* menuWindow;
	Window* miniMapWindow;
	Window* topWindow;
	Window* buildWindow;//TODO uwspolnic z jednostkmai menu, sterowac tylko widocznoscia
	Window* unitsWindow;
	Window* selectedInfoWindow;
	Window* myDebugHud;

	SelectedHudPanel * selectedHudPanel;

	Text* fpsText;
	std::vector<HudElement*>* buttons;
	std::vector<HudElement*>* lists;
	std::vector<Window*>* windows;

	XMLFile* style;
	db_hud_size* hudSize;
	Font* font;
	void createBuildingIcons();
	void createUnitIcons();
	template <std::size_t SIZE>
	void populateList(Font* font, DropDownList* dropDownList, std::array<String, SIZE> elements);

	void initDropDownList(DropDownList* dropDownList);
};
