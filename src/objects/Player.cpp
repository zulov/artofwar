#include "player/Player.h"
#include "Game.h"
#include "ActionType.h"
#include "player/ai/tree/AiOrderType.h"
#include "database/DatabaseCache.h"
#include <string>
#include <utility>
#include "commands/creation/CreationCommandList.h"
#include "simulation/env/Environment.h"
#include <fstream>
#include "stats/Stats.h"


Player::Player(int nationId, int team, int id, int color, Urho3D::String name, bool active): team(team),
                                                                                             name(std::move(name)),
                                                                                             id(id), color(color),
                                                                                             active(active) {
	dbNation = Game::getDatabaseCache()->getNation(nationId);

	std::fill_n(unitLevels, UNITS_NUMBER_DB, 0);
	std::fill_n(buildingLevels, BUILDINGS_NUMBER_DB, 0);
	std::fill_n(unitUpgradeLevels, PATH_UPGRADES_NUMBER_DB, -1);
	initAi();
}

Player::~Player() {
	delete aiRoot;
	delete brain;
}

std::string Player::getValues(int precision) {
	return std::to_string(id) + ","
		+ std::to_string(active) + ","
		+ std::to_string(team) + ","
		+ std::to_string(dbNation->id) + ",'"
		+ name.CString() + "','"
		+ std::to_string(color) + "'";
}

void Player::setResourceAmount(int resource, float amount) {
	resources.setValue(resource, amount);
}

void Player::setResourceAmount(float amount) {
	resources.init(amount);
}

char Player::upgradeLevel(ActionType type, int id) {
	switch (type) {
	case ActionType::UNIT_LEVEL:
		if (Game::getDatabaseCache()->getUnitLevels(id)->size() - 1 > unitLevels[id]) {
			unitLevels[id]++;
			return unitLevels[id];
		}
		break;
	case ActionType::BUILDING_LEVEL:
		if (Game::getDatabaseCache()->getBuildingLevels(id)->size() - 1 > buildingLevels[id]) {
			buildingLevels[id]++;
			return buildingLevels[id];
		}
		break;
	case ActionType::UNIT_UPGRADE:
		if (Game::getDatabaseCache()->getUnitUpgrades(id)->size() - 1 > unitUpgradeLevels[id]) {
			unitUpgradeLevels[id]++;
			return unitUpgradeLevels[id];
		}
		break;
	default: ;
	}
	return -1;
}

void Player::updatePossession() {
	possession.updateAndClean(resources);
	for (auto aiLeaf : aiLeafs) {
		aiLeaf->update(1);
		//aiLeaf->update(possession);
	}
}

void Player::add(Unit* unit) {
	possession.add(unit);
}

void Player::add(Building* building) {
	possession.add(building);
}

int Player::getScore() const {
	return possession.getScore();
}

int Player::getAttackScore() {
	return possession.getAttackScore();
}

int Player::getDefenceScore() {
	return possession.getDefenceScore();
}

int Player::getUnitsNumber() const {
	return possession.getUnitsNumber();
}

int Player::getBuildingsNumber() const {
	return possession.getBuildingsNumber();
}

int Player::getWorkersNumber() const {
	return possession.getWorkersNumber();
}

void Player::ai() {
	auto data= Game::getStats()->getInputFor(getId());
	auto result = brain->decide(data);
	for (int i = 0; i < brain->getOutputSize(); ++i) {
		std::cout << result[i] << std::endl;
	}

	auto& orderData = aiRoot->getOrder();
	execute(orderData);
}

Urho3D::Vector2 Player::bestPosToBuild(const short id) {
	return Game::getEnvironment()->bestPosToBuild(this->id, id);
}

void Player::execute(const AiOrderData& orderData) {
	Game::getLog()->Write(0, "test");
	switch (orderData.type) {
	case AiOrderType::NONE:

		break;
	case AiOrderType::BUILD:
	{
		Urho3D::Vector2 pos = bestPosToBuild(orderData.id);
		Game::getActionCenter()->addBuilding(orderData.id, pos, id, getLevelForBuilding(orderData.id));
	}
	break;
	case AiOrderType::ORDER:

		break;
	case AiOrderType::DEPLOY:

		break;
	default: ;
	}
}

void Player::initAi() {
	aiRoot = new AiNode("root", 100, {AiOrderType::NONE, -1}, nullptr);

	fillAttackNode(aiRoot->addChild("ATTACK", 40, {AiOrderType::NONE, -1}));
	fillDefenseNode(aiRoot->addChild("DEFENSE", 30, {AiOrderType::NONE, -1}));
	fillResourceNode(aiRoot->addChild("RESOURCE", 30, {AiOrderType::NONE, -1}));
	fillIntelNode(aiRoot->addChild("INTEL", 0, {AiOrderType::NONE, -1}));
	std::ifstream infile("Data/ai/w.csv");
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(infile, line)) {
		lines.push_back(line);
	}

	//brain = new Brain(2, 5, 6, 4);
	brain = new Brain(lines);

}

void Player::addBasicNodes(AiNode* parent) {
	parent->addChild("Build", 40, {AiOrderType::BUILD, -1});
	parent->addChild("Order", 30, {AiOrderType::ORDER, -1});
	parent->addChild("Deploy", 30, {AiOrderType::DEPLOY, -1});
}

void Player::mockLeaf(AiNode* parent, std::string name) {
	auto opt1 = parent->getChildByName(name);
	if (opt1.has_value()) {
		opt1.value()->addChild("Mock", 1, {AiOrderType::NONE, -1});
	}
}

void Player::addChild(AiNode* parent, const std::string& name, float targetValue, AiOrderData data) {
	auto node = parent->addChild(name, targetValue, data);
	if (node->getOrder().type != AiOrderType::NONE) {
		aiLeafs.push_back(node);
	}
}

void Player::fillAttackNode(AiNode* parent) {
	addBasicNodes(parent);
	auto opt = parent->getChildByName("Build");
	if (opt.has_value()) {
		auto node = opt.value();
		for (auto building : *Game::getDatabaseCache()->getBuildingForNation(getNation())) {
			node->addChild(building->name.CString(), 10, {AiOrderType::BUILD, building->id});
		}
	}
	mockLeaf(parent, "Order");
	mockLeaf(parent, "Deploy");
}

void Player::fillDefenseNode(AiNode* parent) {
	addBasicNodes(parent);

	mockLeaf(parent, "Build");
	mockLeaf(parent, "Order");
	mockLeaf(parent, "Deploy");
}

void Player::fillResourceNode(AiNode* parent) {
	addBasicNodes(parent);
}

void Player::fillIntelNode(AiNode* parent) {
	addBasicNodes(parent);

	mockLeaf(parent, "Build");
	mockLeaf(parent, "Order");
	mockLeaf(parent, "Deploy");
}
