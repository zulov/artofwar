#pragma once
#include "Benchmark.h"
#include "GameState.h"
#include "camera/CameraManager.h"
#include "control/SelectedInfo.h"
#include "window/debug/DebugPanel.h"
#include "window/in_game_menu/InGameMenuPanel.h"
#include "window/loading/LoadingPanel.h"
#include "window/menu/MenuPanel.h"
#include "window/minimap/MiniMapPanel.h"
#include "window/queue/QueuePanel.h"
#include "window/selected/SelectedHudPanel.h"
#include "window/top/TopPanel.h"
#include "xml/rapidxml_print.hpp"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>


class SimulationInfo;
class MainMenuPanel;
using namespace Urho3D;

class Hud : public Urho3D::Object
{
public:

URHO3D_OBJECT(Hud, Object)

	Hud();
	~Hud();
	void clear();

	void update(Benchmark& benchmark, CameraManager* cameraManager, SelectedInfo* selectedInfo,
                 SimulationInfo* simulationInfo);
	void update(int unitsNumber);
	void createMiniMap();

	void updateSelected(SelectedInfo* selectedInfo);

	void hoverOnIcon(HudData* hudElement);
	void hoverOffIcon(HudData* hudElement);

	std::vector<Button*>& getButtonsSelectedToSubscribe();
	std::vector<HudData*>& getButtonsLeftMenuToSubscribe();

	void HandleUIButtonHoverOn(StringHash, VariantMap& eventData);
	void HandleUIButtonHoverOff(StringHash, VariantMap& eventData);
	void resetLoading();

	void updateLoading(float progress);

	Button* getSaveButton();
	Button* getNewGameProceed();
	Button* getLoadButton();
	Button* getCloseButton();

	void updateStateVisibilty(GameState state);

	void createMyPanels();
	void prepareStyle();
	void preapreUrho();
	void subscribeToUIEvents();
private:
	void replaceVariables(std::string& xml, int hudSizeId);
	void createDebugHud();
	void createConsole();
	void createCursor();

	SelectedHudPanel* selectedHudPanel;
	DebugPanel* debugPanel;
	TopPanel* topPanel;
	MiniMapPanel* miniMapPanel;
	MenuPanel* menuPanel;
	QueuePanel* queuePanel;
	LoadingPanel* loadingPanel;
	InGameMenuPanel* inGameMenuPanel;
	MainMenuPanel* mainMenuPanel;

	std::vector<AbstractWindowPanel*> panels;

	db_graph_settings* graphSettings;
	db_resolution* resolution;
	XMLFile* style;
};
