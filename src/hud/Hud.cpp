#include "Hud.h"
#include "Game.h"
#include <regex>
#include <exprtk/exprtk.hpp>
#include "ButtonUtils.h"
#include "player/PlayersManager.h"
#include "database/DatabaseCache.h"
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <iostream>


void Hud::replaceVariables(std::string& xml, int hudSizeId) {
	exprtk::symbol_table<double> symbol_table;
	int varsSize = Game::get()->getDatabaseCache()->getHudVarsSize();
	for (int i = 0; i < varsSize; ++i) {
		//TODO to lepiej zrobiczapytaniem?
		db_hud_vars* var = Game::get()->getDatabaseCache()->getHudVar(i);
		if (var != nullptr && var->hud_size == hudSizeId) {
			symbol_table.add_variable(var->name.CString(), var->value);
		}
	}
	double resX = (double)graphSettings->res_x;
	double resY = (double)graphSettings->res_y;
	symbol_table.add_variable("resX", resX);
	symbol_table.add_variable("resY", resY);

	typedef exprtk::expression<double> expression_t;
	typedef exprtk::parser<double> parser_t;
	expression_t expression;
	expression.register_symbol_table(symbol_table);

	std::regex reg("\\{[^\\}]*\\}");

	std::regex_iterator<std::string::iterator> iterator(xml.begin(), xml.end(), reg);
	std::regex_iterator<std::string::iterator> rend;
	std::vector<double> values;

	while (iterator != rend) {
		std::string expression_string = iterator->str().substr(1, iterator->str().length() - 2);
		parser_t parser;
		parser.compile(expression_string, expression);
		double y = expression.value();
		values.push_back(y);
		++iterator;
	}

	for (auto var : values) {
		xml = std::regex_replace(xml, reg, std::to_string((int)var), regex_constants::format_first_only);
	}
}

void Hud::createCursor() {
	SharedPtr<Cursor> cursor(new Cursor(Game::get()->getContext()));
	cursor->SetStyleAuto(style);
	Game::get()->getUI()->SetCursor(cursor);
	cursor->SetPosition(Game::get()->getGraphics()->GetWidth() / 2, Game::get()->getGraphics()->GetHeight() / 2);
}

void Hud::createMyPanels() {
	int nation = Game::get()->getPlayersManager()->getActivePlayer()->getNation();

	panels.push_back(selectedHudPanel = new SelectedHudPanel(style));
	panels.push_back(buildPanel = new BuildPanel(style, nation));
	panels.push_back(unitsPanel = new UnitsPanel(style, nation));
	panels.push_back(debugPanel = new DebugPanel(style));
	panels.push_back(topPanel = new TopPanel(style));
	panels.push_back(miniMapPanel = new MiniMapPanel(style));
	panels.push_back(menuPanel = new MenuPanel(style));
	panels.push_back(ordersPanel = new OrdersPanel(style));
	panels.push_back(queuePanel = new QueuePanel(style));
	panels.push_back(loadingPanel = new LoadingPanel(style));
	panels.push_back(inGameMenuPanel = new InGameMenuPanel(style));

	windows = new std::vector<Window*>();

	windows->push_back(menuPanel->createWindow());
	windows->push_back(buildPanel->createWindow());
	windows->push_back(unitsPanel->createWindow());
	windows->push_back(miniMapPanel->createWindow());
	windows->push_back(debugPanel->createWindow());
	windows->push_back(topPanel->createWindow());
	windows->push_back(selectedHudPanel->createWindow());
	windows->push_back(ordersPanel->createWindow());
	windows->push_back(queuePanel->createWindow());
	windows->push_back(loadingPanel->createWindow());
	windows->push_back(inGameMenuPanel->createWindow());

	for (auto panel : panels) {
		panel->setVisible(false);
	}
}

void Hud::prepareStyle() {
	vector<char *> sth;
	rapidxml::xml_document<> baseXML;
	rapidxml::xml_node<>* a = baseXML.allocate_node(rapidxml::node_element, "elements");
	baseXML.append_node(a);

	for (int i = 0; i < graphSettings->styles.Size(); ++i) {
		XMLFile* style2 = Game::get()->getCache()->GetResource<XMLFile>("UI/" + graphSettings->styles[i]);
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
	style = new XMLFile(Game::get()->getContext());

	style->FromString(result_xml.c_str());//TODO moze problem z pamiecia
	Game::get()->getUI()->GetRoot()->SetDefaultStyle(style);
}

void Hud::preapreUrho() {
	createConsole();
	createDebugHud();
	createCursor();
}

Hud::Hud() {
	graphSettings = Game::get()->getDatabaseCache()->getGraphSettings(0);

	prepareStyle();
}


void Hud::clear() {
	clear_vector(panels);
	delete windows;
	Game::get()->getUI()->GetCursor()->Remove();
	Game::get()->getUI()->GetRoot()->RemoveAllChildren();
}

Hud::~Hud() {
	clear();
	//Game::get()->getUI()->GetRoot()->Remove();
}

void Hud::createDebugHud() {
	DebugHud* debugHud = Game::get()->getEngine()->CreateDebugHud();
	debugHud->SetDefaultStyle(style);
}

void Hud::createConsole() {
	Console* console = Game::get()->getEngine()->CreateConsole();
	console->SetDefaultStyle(style);
	console->GetBackground()->SetOpacity(0.8f);
}

void Hud::update(Benchmark* benchmark, CameraManager* cameraManager) {
	debugPanel->setText(benchmark->getLastFPS(), benchmark->getAverageFPS(), benchmark->getLoops(),
	                    cameraManager->getInfo());

	topPanel->update(Game::get()->getPlayersManager()->getActivePlayer()->getResources());
	miniMapPanel->update();
}

void Hud::update(int unitsNumber) {
	topPanel->update(unitsNumber);
}

void Hud::createMiniMap() {
	miniMapPanel->createEmpty(160);
}

std::vector<HudElement*>* Hud::getButtonsBuildToSubscribe() {
	return buildPanel->getButtons();
}

std::vector<HudElement*>* Hud::getButtonsUnitsToSubscribe() {
	return unitsPanel->getButtons();
}

std::vector<Window*>* Hud::getWindows() {
	return windows;
}

void Hud::resetLoading() {
	for (auto panel : panels) {
		panel->setVisible(false);
	}
	loadingPanel->show();
}

void Hud::endLoading() {
	for (auto panel : panels) {
		panel->setVisible(true);
	}
	loadingPanel->end();
}

void Hud::updateLoading(float progress) {
	loadingPanel->update(progress);
}

Sprite* Hud::getSpriteMiniMapToSubscribe() {
	return miniMapPanel->getSpriteToSubscribe();
}

Button* Hud::getSaveButton() {
	return inGameMenuPanel->getSaveButton();
}

void Hud::updateSelected(SelectedInfo* selectedInfo) {
	//TODO raz stworzyc a sterowac widzialnsocia
	if (selectedInfo->hasChanged()) {
		selectedHudPanel->update(selectedInfo);
		menuPanel->updateSelected(selectedInfo);
		switch (selectedInfo->getSelectedType()) {

		case UNIT:
			ordersPanel->show(selectedInfo);
			buildPanel->setVisible(false);
			unitsPanel->setVisible(false);
			queuePanel->setVisible(false);
			break;
		case BUILDING:
			ordersPanel->setVisible(false);
			buildPanel->setVisible(false);
			unitsPanel->show(selectedInfo);
			queuePanel->show(selectedInfo);
			break;
		default:
			ordersPanel->setVisible(false);
			buildPanel->show();
			unitsPanel->setVisible(false);
			queuePanel->setVisible(false);
		}
	} else {
		queuePanel->update(selectedInfo);
	}
}

std::vector<Button*>* Hud::getButtonsSelectedToSubscribe() {
	return selectedHudPanel->getButtonsSelectedToSubscribe();
}

void Hud::hoverOnIcon(HudElement* hudElement) {
	menuPanel->setInfo(hudElement);
}

void Hud::hoverOffIcon(HudElement* hudElement) {
	menuPanel->removeInfo();
}

std::vector<HudElement*>* Hud::getButtonsOrdersToSubscribe() {
	return ordersPanel->getButtons();
}
