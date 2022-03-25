#include "Hud.h"
#include <exprtk/exprtk.hpp>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include "Benchmark.h"
#include "Game.h"
#include "HudData.h"
#include "camera/CameraManager.h"
#include "control/SelectedInfo.h"
#include "database/DatabaseCache.h"
#include "database/db_grah_structs.h"
#include "objects/ObjectEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/PerFrameAction.h"
#include "simulation/SimInfo.h"
#include "utils/replace_utils.h"
#include "window/debug/DebugPanel.h"
#include "window/loading/LoadingPanel.h"
#include "window/minimap/MiniMapPanel.h"
#include "window/queue/QueuePanel.h"
#include "window/score/ScorePanel.h"
#include "window/top/TopPanel.h"

void Hud::replaceVariables(std::string& xml, int hudSizeId) const {
	exprtk::symbol_table<float> symbol_table;
	for (auto var : Game::getDatabase()->getHudVars()) {
		if (var != nullptr && var->hud_size == hudSizeId) {
			symbol_table.add_variable(var->name.CString(), var->value);
		}
	}
	float resX = resolution->x;
	float resY = resolution->y;
	symbol_table.add_variable("resX", resX);
	symbol_table.add_variable("resY", resY);

	using expression_t = exprtk::expression<float>;
	using parser_t = exprtk::parser<float>;
	expression_t expression;
	expression.register_symbol_table(symbol_table);

	std::regex reg(R"(\{[^\}]*\})");

	std::regex_iterator<std::string::iterator> iterator(xml.begin(), xml.end(), reg);
	std::regex_iterator<std::string::iterator> rend;
	std::vector<int> values;

	parser_t parser;

	while (iterator != rend) {
		std::string expression_string = iterator->str().substr(1, iterator->str().length() - 2);

		parser.compile(expression_string, expression);
		values.push_back((int)expression.value());
		++iterator;
	}

	xml = regex_replace(xml, reg, values);
}

void Hud::createCursor() const {
	Urho3D::SharedPtr<Urho3D::Cursor> cursor(new Urho3D::Cursor(Game::getContext()));
	cursor->SetStyleAuto(style);

	Game::getUI()->SetCursor(cursor);

	auto graphics = Game::getGraphics();
	if (graphics) {
		cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);
	}

}

void Hud::createMyPanels() {
	panels = {
		selectedHudPanel = new SelectedHudPanel(style),
		debugPanel = new DebugPanel(style),
		topPanel = new TopPanel(style),
		menuPanel = new MenuPanel(style),
		queuePanel = new QueuePanel(style),
		loadingPanel = new LoadingPanel(style),
		inGameMenuPanel = new InGameMenuPanel(style),
		mainMenuPanel = new MainMenuPanel(style),
		miniMapPanel = new MiniMapPanel(style),
		scorePanel = new ScorePanel(style)
	};

	for (auto panel : panels) {
		panel->createWindow();
		panel->setVisible(false);
	}
}

void Hud::prepareStyle() {
	Urho3D::String body = "<elements>";

	for (const auto& sett : graphSettings->styles) {
		auto style2 = Game::getCache()->GetResource<Urho3D::XMLFile>("UI/" + sett);

		body.Append(style2->ToString());
		Game::getCache()->ReleaseResource<Urho3D::XMLFile>("UI/" + sett);
	}
	body.Append("</elements>");

	std::string result_xml = body.CString();

	replaceVariables(result_xml, graphSettings->hud_size);
	style = new Urho3D::XMLFile(Game::getContext());

	style->FromString(result_xml.c_str()); //TODO moze problem z pamiecia
	Game::getUI()->GetRoot()->SetDefaultStyle(style);
}

void Hud::prepareUrho(Urho3D::Engine* engine) const {
	createConsole(engine);
	engine->CreateDebugHud()->SetDefaultStyle(style);
	createCursor();
}

void Hud::subscribeToUIEvents() {
	for (auto hudElement : getButtonsLeftMenuToSubscribe()) {
		auto element = hudElement->getUIParent();
		SubscribeToEvent(element, Urho3D::E_HOVERBEGIN, URHO3D_HANDLER(Hud, HandleUIButtonHoverOn));
		SubscribeToEvent(element, Urho3D::E_HOVEREND, URHO3D_HANDLER(Hud, HandleUIButtonHoverOff));
	}
}

Hud::Hud() : Object(Game::getContext()) {
	if (!SIM_GLOBALS.HEADLESS) {
		db_settings* settings = Game::getDatabase()->getSettings();
		graphSettings = Game::getDatabase()->getGraphSettings()[settings->graph];
		resolution = Game::getDatabase()->getResolution(settings->resolution);

		prepareStyle();
	}
	std::ranges::fill(panels, nullptr);
}


void Hud::clear() {
	clear_array(panels);

	Game::getUI()->GetRoot()->RemoveAllChildren();
}

Hud::~Hud() {
	clear();
	if (Game::getUI()->GetCursor() != nullptr) {
		Game::getUI()->GetCursor()->Remove();
	}
}

void Hud::createConsole(Urho3D::Engine* engine) const {
	auto console = engine->CreateConsole();
	if (console) {
		console->SetDefaultStyle(style);
		console->GetBackground()->SetOpacity(0.8f);
	}
}

void Hud::update(Benchmark& benchmark, CameraManager* cameraManager, SelectedInfo* selectedInfo,
                 SimInfo* simInfo) const {
	updateSelected(selectedInfo, simInfo);

	debugPanel->setText(simInfo->getFrameInfo()->getSeconds(),
	                    benchmark.getAvgLowest(), benchmark.getAvgMiddle(), benchmark.getAvgHighest(),
	                    cameraManager->getPosInfo());
	if (PER_FRAME_ACTION.get(PerFrameAction::HUD_UPDATE, simInfo->getFrameInfo())) {
		topPanel->update(Game::getPlayersMan()->getActivePlayer());

		scorePanel->update(Game::getPlayersMan()->getAllPlayers());
	}
	miniMapPanel->update();

	selectedInfo->hasBeenUpdatedDrawn();
}

void Hud::createMiniMap() const {
	if (miniMapPanel) {
		miniMapPanel->createEmpty(160);
	}
}

void Hud::resetLoading() const {
	if (loadingPanel) {
		loadingPanel->show();
	}
}

void Hud::updateLoading(float progress) const {
	if (loadingPanel) {
		loadingPanel->update(progress);
	}
}

void Hud::updateStateVisibilty(GameState state) {
	for (auto panel : panels) {
		if (panel) {
			panel->updateStateVisibility(state);
		}
	}
}

void Hud::updateSelected(SelectedInfo* selectedInfo, SimInfo* simInfo) const {
	if (selectedInfo->isSthSelected() || selectedInfo->hasChanged()) {
		if (selectedInfo->hasChanged()
			|| PER_FRAME_ACTION.get(PerFrameAction::QUEUE_HUD, simInfo->getFrameInfo())) {
			selectedHudPanel->update(selectedInfo);
			menuPanel->updateSelected(selectedInfo);
			switch (selectedInfo->getSelectedType()) {
			case ObjectType::NONE:
				menuPanel->refresh(LeftMenuMode::BUILDING, selectedInfo);
				queuePanel->show(Game::getPlayersMan()->getActivePlayer()->getQueue());
				break;
			case ObjectType::UNIT:
				menuPanel->refresh(LeftMenuMode::ORDER, selectedInfo);
				queuePanel->setVisible(false);
				break;
			case ObjectType::BUILDING:
				menuPanel->refresh(LeftMenuMode::UNIT, selectedInfo);
				queuePanel->show(selectedInfo);
				break;
			case ObjectType::RESOURCE:
				menuPanel->refresh(LeftMenuMode::RESOURCE, selectedInfo);
				queuePanel->setVisible(false);
				break;
			}
		}
		if (selectedInfo->getSelectedType() == ObjectType::BUILDING) {
			queuePanel->update(selectedInfo);
		}
	} else {
		if (PER_FRAME_ACTION.get(PerFrameAction::QUEUE_HUD, simInfo->getFrameInfo())) {
			queuePanel->show(Game::getPlayersMan()->getActivePlayer()->getQueue());
		}

		selectedHudPanel->clearSelected();
		menuPanel->clearSelected();
	}
}

void Hud::HandleUIButtonHoverOn(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	hoverOnIcon(HudData::getFromElement(eventData));
}

void Hud::HandleUIButtonHoverOff(Urho3D::StringHash /*eventType*/, Urho3D::VariantMap& eventData) {
	hoverOffIcon();
}
