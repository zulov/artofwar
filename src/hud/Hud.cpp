#include "Hud.h"
#include <exprtk/exprtk.hpp>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <xml/rapidxml_print.hpp>
#include "Benchmark.h"
#include "Game.h"
#include "HudData.h"
#include "camera/CameraManager.h"
#include "control/SelectedInfo.h"
#include "database/DatabaseCache.h"
#include "objects/ObjectEnums.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "simulation/SimulationInfo.h"
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
	cursor->SetPosition(Game::getGraphics()->GetWidth() / 2, Game::getGraphics()->GetHeight() / 2);
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
	std::vector<char*> sth;
	rapidxml::xml_document<> baseXML;
	rapidxml::xml_node<>* a = baseXML.allocate_node(rapidxml::node_element, "elements");
	baseXML.append_node(a);

	for (const auto& sett : graphSettings->styles) {
		auto style2 = Game::getCache()->GetResource<Urho3D::XMLFile>("UI/" + sett);
		rapidxml::xml_document<> additionalXML;
		auto chs = _strdup(style2->ToString().CString());
		sth.push_back(chs);
		additionalXML.parse<0>(chs);

		rapidxml::xml_node<>* root = additionalXML.first_node();

		for (rapidxml::xml_node<>* node = root->first_node(); node; node = node->next_sibling()) {
			rapidxml::xml_node<>* clone = baseXML.clone_node(node);
			baseXML.first_node()->append_node(clone);
		}
	}

	std::stringstream ss;
	ss << *baseXML.first_node();
	std::string result_xml = ss.str();
	for (auto value : sth) {
		free(value);
	}

	replaceVariables(result_xml, graphSettings->hud_size);
	style = new Urho3D::XMLFile(Game::getContext());

	style->FromString(result_xml.c_str()); //TODO moze problem z pamiecia
	Game::getUI()->GetRoot()->SetDefaultStyle(style);
}

void Hud::prepareUrho() const {
	createConsole();
	createDebugHud();
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
	db_settings* settings = Game::getDatabase()->getSettings();
	graphSettings = Game::getDatabase()->getGraphSettings()[settings->graph];
	resolution = Game::getDatabase()->getResolution(settings->resolution);

	prepareStyle();
}


void Hud::clear() {
	clear_array(panels);

	Game::getUI()->GetRoot()->RemoveAllChildren();
}

Hud::~Hud() {
	clear();

	Game::getUI()->GetCursor()->Remove();
}

void Hud::createDebugHud() const {
	auto debugHud = Game::getEngine()->CreateDebugHud();
	debugHud->SetDefaultStyle(style);
}

void Hud::createConsole() const {
	auto console = Game::getEngine()->CreateConsole();
	console->SetDefaultStyle(style);
	console->GetBackground()->SetOpacity(0.8f);
}

void Hud::update(Benchmark& benchmark, CameraManager* cameraManager, SelectedInfo* selectedInfo,
                 SimulationInfo* simulationInfo) const {
	updateSelected(selectedInfo, simulationInfo->getCurrentFrame());

	debugPanel->setText(benchmark.getLastFPS(), benchmark.getAverageFPS(), benchmark.getLoops(),
	                    benchmark.getAvgLowest(), benchmark.getAvgMiddle(), benchmark.getAvgHighest(),
	                    cameraManager->getInfo());

	topPanel->update(Game::getPlayersMan()->getActivePlayer());

	scorePanel->update(Game::getPlayersMan()->getAllPlayers());
	miniMapPanel->update();
	selectedInfo->hasBeenUpdatedDrawn();
}

void Hud::createMiniMap() const {
	miniMapPanel->createEmpty(160);
}

void Hud::resetLoading() const {
	loadingPanel->show();
}

void Hud::updateLoading(float progress) const {
	loadingPanel->update(progress);
}

void Hud::updateStateVisibilty(GameState state) {
	for (auto panel : panels) {
		panel->updateStateVisibilty(state);
	}
}

void Hud::updateSelected(SelectedInfo* selectedInfo, int currentFrame) const {
	if (selectedInfo->isSthSelected() || selectedInfo->hasChanged()) {
		if (selectedInfo->hasChanged() || currentFrame % 10 == 0) {
			selectedHudPanel->update(selectedInfo);
			menuPanel->updateSelected(selectedInfo);
			switch (selectedInfo->getSelectedType()) {
			case ObjectType::PHYSICAL:
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

		if (currentFrame % 10 == 0) {
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
