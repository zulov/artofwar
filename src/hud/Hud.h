#pragma once
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Sprite.h>
#include "camera/CameraManager.h"
#include "Benchmark.h"
#include "HudElement.h"
#include "database/db_strcut.h"
#include "control/SelectedInfo.h"
#include "window/selected/SelectedHudPanel.h"
#include "window/building/BuildPanel.h"
#include "window/units/UnitsPanel.h"
#include "window/debug/DebugPanel.h"
#include "window/menu/MenuPanel.h"
#include "window/top/TopPanel.h"
#include "window/minimap/MiniMapPanel.h"
#include "window/orders/OrdersPanel.h"
#include "window/queue/QueuePanel.h"
#include "window/loading/LoadingPanel.h"
#include "window/in_game_menu/InGameMenuPanel.h"
#include "xml/rapidxml.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_iterators.hpp"

using namespace Urho3D;

class Hud
{
public:
	Hud();
	~Hud();

	void update(Benchmark* benchmark, CameraManager* cameraManager);
	void update(int unitsNumber);
	void createMiniMap();
	void updateSelected(SelectedInfo* selectedInfo);

	void hoverOnIcon(HudElement* hudElement);
	void hoverOffIcon(HudElement* hudElement);

	std::vector<Button*>* getButtonsSelectedToSubscribe();
	std::vector<HudElement*>* getButtonsOrdersToSubscribe();
	std::vector<HudElement*>* getButtonsBuildToSubscribe();
	std::vector<HudElement*>* getButtonsUnitsToSubscribe();
	std::vector<Window*>* getWindows();
	void resetLoading();
	void endLoading();
	void updateLoading(float progress);
	Sprite* getSpriteMiniMapToSubscribe();

private:
	void replaceVariables(XMLFile* xmlFile, int hudSizeId);
	void createDebugHud();
	void createConsole();
	void createCursor();

	SelectedHudPanel* selectedHudPanel;
	BuildPanel* buildPanel;
	UnitsPanel* unitsPanel;
	DebugPanel* debugPanel;
	TopPanel* topPanel;
	MiniMapPanel* miniMapPanel;
	MenuPanel* menuPanel;
	OrdersPanel* ordersPanel;
	QueuePanel* queuePanel;
	LoadingPanel* loadingPanel;
	InGameMenuPanel * inGameMenuPanel;

	std::vector<AbstractWindowPanel*> panels;

	std::vector<Window*>* windows;
	db_graph_settings* graphSettings;
	XMLFile* style;

};
