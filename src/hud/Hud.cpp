#include "Hud.h"
#include "Game.h"
#include "HudData.h"
#include "ObjectEnums.h"
#include "UiUtils.h"
#include "camera/CameraManager.h"
#include "database/DatabaseCache.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "replace_utils.h"
#include "simulation/SimulationInfo.h"
#include "utils.h"
#include "window/debug/DebugPanel.h"
#include "window/loading/LoadingPanel.h"
#include "window/minimap/MiniMapPanel.h"
#include "window/queue/QueuePanel.h"
#include "window/top/TopPanel.h"
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <exprtk/exprtk.hpp>
#include <regex>


void Hud::replaceVariables(std::string& xml, int hudSizeId) {
	exprtk::symbol_table<float> symbol_table;
	int varsSize = Game::getDatabaseCache()->getHudVarsSize();
	for (int i = 0; i < varsSize; ++i) {
		//TODO to lepiej zrobiczapytaniem?
		db_hud_vars* var = Game::getDatabaseCache()->getHudVar(i);
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

void Hud::createCursor() {
	SharedPtr<Cursor> cursor(new Cursor(Game::getContext()));
	cursor->SetStyleAuto(style);
	Game::getUI()->SetCursor(cursor);
	cursor->SetPosition(Game::getGraphics()->GetWidth() / 2, Game::getGraphics()->GetHeight() / 2);
}

void Hud::createMyPanels() {

	panels.push_back(selectedHudPanel = new SelectedHudPanel(style));
	panels.push_back(debugPanel = new DebugPanel(style));
	panels.push_back(topPanel = new TopPanel(style));
	panels.push_back(menuPanel = new MenuPanel(style));
	panels.push_back(queuePanel = new QueuePanel(style));
	panels.push_back(loadingPanel = new LoadingPanel(style));
	panels.push_back(inGameMenuPanel = new InGameMenuPanel(style));
	panels.push_back(mainMenuPanel = new MainMenuPanel(style));
	panels.push_back(miniMapPanel = new MiniMapPanel(style));

	for (auto panel : panels) {
		panel->createWindow();
		panel->setVisible(false);
	}
}

void Hud::prepareStyle() {
	vector<char *> sth;
	rapidxml::xml_document<> baseXML;
	rapidxml::xml_node<>* a = baseXML.allocate_node(rapidxml::node_element, "elements");
	baseXML.append_node(a);

	for (int i = 0; i < graphSettings->styles.Size(); ++i) {
		XMLFile* style2 = Game::getCache()->GetResource<XMLFile>("UI/" + graphSettings->styles[i]);
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
	style = new XMLFile(Game::getContext());

	style->FromString(result_xml.c_str()); //TODO moze problem z pamiecia
	Game::getUI()->GetRoot()->SetDefaultStyle(style);
}

void Hud::preapreUrho() {
	createConsole();
	createDebugHud();
	createCursor();
}

void Hud::subscribeToUIEvents() {
	for (auto hudElement : getButtonsLeftMenuToSubscribe()) {
		UIElement* element = hudElement->getUIParent();
		SubscribeToEvent(element, E_HOVERBEGIN, URHO3D_HANDLER(Hud, HandleUIButtonHoverOn));
		SubscribeToEvent(element, E_HOVEREND, URHO3D_HANDLER(Hud, HandleUIButtonHoverOff));
	}
}

Hud::Hud() : Object(Game::getContext()) {
	db_settings* settings = Game::getDatabaseCache()->getSettings();
	graphSettings = Game::getDatabaseCache()->getGraphSettings(settings->graph);
	resolution = Game::getDatabaseCache()->getResolution(settings->resolution);

	prepareStyle();
}


void Hud::clear() {
	clear_vector(panels);

	Game::getUI()->GetRoot()->RemoveAllChildren();
}

Hud::~Hud() {
	clear();

	Game::getUI()->GetCursor()->Remove();
}

void Hud::createDebugHud() {
	DebugHud* debugHud = Game::getEngine()->CreateDebugHud();
	debugHud->SetDefaultStyle(style);
}

void Hud::createConsole() {
	Console* console = Game::getEngine()->CreateConsole();
	console->SetDefaultStyle(style);
	console->GetBackground()->SetOpacity(0.8f);
}

void Hud::update(Benchmark& benchmark, CameraManager* cameraManager, SelectedInfo* selectedInfo,
                 SimulationInfo* simulationInfo) {
	updateSelected(selectedInfo);
	if (simulationInfo->ifAmountUnitChanged()) {
		update(simulationInfo->getUnitsNumber());
	}

	debugPanel->setText(benchmark.getLastFPS(), benchmark.getAverageFPS(), benchmark.getLoops(),
	                    cameraManager->getInfo());

	topPanel->update(Game::getPlayersManager()->getActivePlayer()->getResources());
	topPanel->update(simulationInfo->getUnitsNumber());
	miniMapPanel->update();
	selectedInfo->hasBeedUpdatedDrawn();
}

void Hud::update(int unitsNumber) {
	topPanel->update(unitsNumber);
}

void Hud::createMiniMap() {
	miniMapPanel->createEmpty(160);
}

void Hud::resetLoading() {
	loadingPanel->show();
}

void Hud::updateLoading(float progress) {
	loadingPanel->update(progress);
}

void Hud::updateStateVisibilty(GameState state) {
	for (auto panel : panels) {
		panel->updateStateVisibilty(state);
	}
}

void Hud::updateSelected(SelectedInfo* selectedInfo) {
	//TODO raz stworzyc a sterowac widzialnsocia
	if (selectedInfo->hasChanged()) {
		selectedHudPanel->update(selectedInfo);
		menuPanel->updateSelected(selectedInfo);
		switch (selectedInfo->getSelectedType()) {
		case ObjectType::PHYSICAL:
			menuPanel->refresh(LeftMenuMode::BUILDING, selectedInfo);
			queuePanel->show(Game::getQueueManager());
			break;
		case ObjectType::UNIT:
			menuPanel->refresh(LeftMenuMode::ORDER, selectedInfo);
			queuePanel->setVisible(false);
			break;
		case ObjectType::BUILDING:
			menuPanel->refresh(LeftMenuMode::UNIT, selectedInfo);
			queuePanel->show(selectedInfo);
			break;
		default:
			menuPanel->refresh(LeftMenuMode::BUILDING, selectedInfo);
			queuePanel->setVisible(false);
		}
	} else {
		switch (selectedInfo->getSelectedType()) {

		case ObjectType::PHYSICAL:
			queuePanel->update(Game::getQueueManager());
			break;
		case ObjectType::BUILDING:
			queuePanel->update(selectedInfo);
			break;
		}
	}
}

void Hud::HandleUIButtonHoverOn(StringHash /*eventType*/, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudData* hudElement = static_cast<HudData *>(element->GetVar("HudElement").GetVoidPtr());
	hoverOnIcon(hudElement);
}

void Hud::HandleUIButtonHoverOff(StringHash /*eventType*/, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudData* hudElement = static_cast<HudData *>(element->GetVar("HudElement").GetVoidPtr());
	hoverOffIcon(hudElement);
}
