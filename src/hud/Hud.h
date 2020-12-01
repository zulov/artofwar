#pragma once
#include <Urho3D/Core/Object.h>
#include "window/in_game_menu/InGameMenuPanel.h"
#include "window/main_menu/MainMenuPanel.h"
#include "window/menu/MenuPanel.h"
#include "window/selected/SelectedHudPanel.h"

namespace Urho3D
{
	class Engine;
	class XMLFile;
	class Button;
}

class SimplePanel;
class Benchmark;
class ScorePanel;
class SelectedHudPanel;
class SelectedInfo;
class CameraManager;
class SimulationInfo;
class MiniMapPanel;
class TopPanel;
class HudData;
class DebugPanel;
class QueuePanel;
class LoadingPanel;

struct db_graph_settings;
struct db_resolution;

class Hud : public Urho3D::Object
{
public:

URHO3D_OBJECT(Hud, Object)

	Hud();
	~Hud();
	void clear();

	void update(Benchmark& benchmark, CameraManager* cameraManager, SelectedInfo* selectedInfo,
	            SimulationInfo* simulationInfo) const;
	void createMiniMap() const;

	void updateSelected(SelectedInfo* selectedInfo) const;

	void hoverOnIcon(HudData* hudElement) const { menuPanel->setHoverInfo(hudElement); }
	void hoverOffIcon() const { menuPanel->removeHoverInfo(); }

	void HandleUIButtonHoverOn(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void HandleUIButtonHoverOff(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	void resetLoading() const;

	void updateLoading(float progress) const;

	void updateStateVisibilty(GameState state);

	void createMyPanels();
	void prepareStyle();
	void prepareUrho(Urho3D::Engine* engine) const;
	void subscribeToUIEvents();

	std::vector<Urho3D::Button*> getButtonsSelectedToSubscribe() const
	{
		return selectedHudPanel->getButtonsSelectedToSubscribe();
	}

	std::vector<HudData*>& getButtonsLeftMenuToSubscribe() const { return menuPanel->getButtons(); }

	Urho3D::Button* getSaveButton() const { return inGameMenuPanel->getSaveButton(); }
	Urho3D::Button* getNewGameProceed() const { return mainMenuPanel->getNewGameProceed(); }
	Urho3D::Button* getLoadButton() const { return mainMenuPanel->getLoadButton(); }
	Urho3D::Button* getCloseButton() const { return mainMenuPanel->getCloseButton(); }
private:

	void replaceVariables(std::string& xml, int hudSizeId) const;
	void createDebugHud(Urho3D::Engine* engine) const;
	void createConsole(Urho3D::Engine* engine) const;
	void createCursor() const;

	SelectedHudPanel* selectedHudPanel;
	DebugPanel* debugPanel;
	TopPanel* topPanel;
	MiniMapPanel* miniMapPanel;
	MenuPanel* menuPanel;
	QueuePanel* queuePanel;
	LoadingPanel* loadingPanel;
	InGameMenuPanel* inGameMenuPanel;
	MainMenuPanel* mainMenuPanel;
	ScorePanel* scorePanel;

	std::array<SimplePanel*, 10> panels;

	db_graph_settings* graphSettings;
	db_resolution* resolution;
	Urho3D::XMLFile* style;
};
