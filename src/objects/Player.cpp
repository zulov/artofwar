#include "player/Player.h"
#include "Game.h"
#include "ActionType.h"
#include "player/ai/tree/AiOrderType.h"
#include "database/DatabaseCache.h"
#include <string>


Player::Player(int nationId, int team, int id, int color, const Urho3D::String& name, bool active): team(team), id(id),
                                                                                                    color(color),
                                                                                                    name(name),
                                                                                                    active(active) {
	dbNation = Game::getDatabaseCache()->getNation(nationId);

	std::fill_n(unitLevels, UNITS_NUMBER_DB, 0);
	std::fill_n(buildingLevels, BUILDINGS_NUMBER_DB, 0);
	std::fill_n(unitUpgradeLevels, PATH_UPGRADES_NUMBER_DB, -1);
	initAi();
}

Player::~Player() {
	delete aiRoot;
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

int Player::getScore() {
	return possession.getScore();
}

void Player::ai() {
	aiRoot->getOrder();
}

void Player::initAi() {
	aiRoot = new AiNode("root", 100, {AiOrderType::NONE, -1}, nullptr);
	auto attackNode = aiRoot->addChild("ATTACK", 30, {AiOrderType::NONE, -1});
	auto defenseNode = aiRoot->addChild("DEFENSE", 30, {AiOrderType::NONE, -1});
	auto resourceNode = aiRoot->addChild("RESOURCE", 40, {AiOrderType::NONE, -1});
	auto intelNode = aiRoot->addChild("INTEL", 0, {AiOrderType::NONE, -1});

	fillAttackNode(attackNode);
	fillDefenseNode(defenseNode);
	fillResourceNode(resourceNode);
	fillIntelNode(intelNode);
}

void Player::fillAttackNode(AiNode& parent) {
	auto build = parent.addChild("Build", 30, {AiOrderType::BUILD, -1});
	auto order = parent.addChild("Order", 40, {AiOrderType::ORDER, -1});
	auto deploy = parent.addChild("Deploy", 30, {AiOrderType::DEPLOY, -1});
}

void Player::fillDefenseNode(AiNode& parent) {
	auto build = parent.addChild("Build", 30, {AiOrderType::BUILD, -1});
	auto order = parent.addChild("Order", 40, {AiOrderType::ORDER, -1});
	auto deploy = parent.addChild("Deploy", 30, {AiOrderType::DEPLOY, -1});
}

void Player::fillResourceNode(AiNode& parent) {
	auto build = parent.addChild("Build", 30, {AiOrderType::BUILD, -1});
	auto order = parent.addChild("Order", 40, {AiOrderType::ORDER, -1});
	auto deploy = parent.addChild("Deploy", 30, {AiOrderType::DEPLOY, -1});
}

void Player::fillIntelNode(AiNode& parent) {
	auto build = parent.addChild("Build", 30, {AiOrderType::BUILD, -1});
	auto order = parent.addChild("Order", 40, {AiOrderType::ORDER, -1});
	auto deploy = parent.addChild("Deploy", 30, {AiOrderType::DEPLOY, -1});
}
