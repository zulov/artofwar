#include "player/Player.h"

#include <algorithm>
#include <cmath>
#include <ranges>

#include "Game.h"
#include "Possession.h"
#include "Resources.h"
#include "database/DatabaseCache.h"
#include "database/db_technology_struct.h"
#include "database/db_struct.h"
#include "env/Environment.h"
#include "objects/queue/QueueActionType.h"
#include "objects/building/Building.h"
#include "simulation/WorldAgeController.h"
#include "utils/TechnologyUtils.h"

Player::Player(unsigned char nationId, unsigned char team, unsigned char id, unsigned char color, Urho3D::String name,
			   bool active, unsigned currentBuildingUId, unsigned currentUnitUId) :
	team(team), id(id), active(active), color(color), currentBuildingUId(currentBuildingUId),
	currentUnitUId(currentUnitUId), dbNation(Game::getDatabase()->getNation(nationId)),
	possession(new Possession(nationId)), resources(new Resources()), aiOrchestrator(this, dbNation, &aiHistory),
	name(std::move(name)) {
	technologyLevels.assign(Game::getDatabase()->getTechnologies().size(), 0);
	unitLevels.resize(Game::getDatabase()->getUnits().size());
	buildingLevels.resize(Game::getDatabase()->getBuildings().size());
	refreshEffectiveLevels();
}

Player::~Player() {
	for (auto& level : unitLevels) delete level.effective;
	for (auto& level : buildingLevels) delete level.effective;
	delete resources;
	delete possession;
}

void Player::setResourceAmount(float food, float wood, float stone, float gold) const {
	resources->setValue(food, wood, stone, gold);
}

void Player::setResourceAmount(float amount) const { resources->init(amount); }

char Player::upgradeLevel(QueueActionType type, unsigned short levelId) {
	switch (type) {
	case QueueActionType::UNIT_LEVEL:
		if (id < unitLevels.size() && unitLevels[id].id >= 0) {
			if (const auto next = getNextUnitLevel(levelId);
				Game::getWorldAgeController()->isLevelAvailable((*next)->ageStage)) {
				++unitLevels[id].level;
				refreshEffectiveLevels();
				return unitLevels[id].level;
			}
		}
		break;
	case QueueActionType::BUILDING_LEVEL:
		if (id < buildingLevels.size() && buildingLevels[id].id >= 0) {
			if (const auto next = getNextBuildingLevel(levelId);
				Game::getWorldAgeController()->isLevelAvailable((*next)->ageStage)) {
				++buildingLevels[id].level;
				refreshEffectiveLevels();
				return buildingLevels[id].level;
			}
		}
		break;
	default:;
	}
	return -1;
}

unsigned char Player::getNation() const { return dbNation->id; }

db_unit_level* Player::getUnitLevel(unsigned short id) const {
	return id < unitLevels.size() ? unitLevels[id].effective : nullptr;
}

bool Player::startTechnologyResearch(unsigned short levelId, Building* building) {
	if (!canResearchTechnology(levelId, building)) return false;

	const auto effectiveCost = technologyResearchCost(levelId);
	if (!resources->reduce(&effectiveCost)) return false;
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	const auto duration = technologyResearchDuration(levelId);
	building->getQueue().add(QueueActionType::TECH_RESEARCH, levelId, 1, duration);
	return true;
}

unsigned short Player::technologyResearchDuration(unsigned short levelId) const {
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	return static_cast<unsigned short>(std::max(1.f, std::round(level->researchTime * technologyAgeMultiplier(level))));
}

db_building_level* Player::getBuildingLevel(unsigned short id) const {
	return id < buildingLevels.size() ? buildingLevels[id].effective : nullptr;
}

bool Player::isBuildingAvailable(unsigned short id) const { return isAvailable(Game::getDatabase()->getBuilding(id)); }
 
bool Player::isAvailable(const db_building* dbBuilding) const {
	return Game::getWorldAgeController()->isLevelAvailable(dbBuilding->levels.front()->ageStage);
}

std::optional<db_unit_level*> Player::getNextUnitLevel(unsigned short id) const {
	if (id >= unitLevels.size() || unitLevels[id].id < 0) return std::nullopt;
	const auto* unit = Game::getDatabase()->getUnit(id);
	const auto nextLevel = static_cast<size_t>(unitLevels[id].level) + 1;
	return unit && nextLevel < unit->levels.size() ? std::optional(unit->levels[nextLevel]) : std::nullopt;
}

std::optional<db_building_level*> Player::getNextBuildingLevel(unsigned short id) const {
	if (id >= buildingLevels.size() || buildingLevels[id].id < 0) return std::nullopt;
	return getNextLevel(Game::getDatabase()->getBuilding(id));
}

std::optional<db_building_level*> Player::getNextLevel(db_building* dbBuilding) const {
	const auto nextLevel = static_cast<size_t>(buildingLevels[dbBuilding->id].level) + 1;
	return nextLevel < dbBuilding->levels.size() ? std::optional(dbBuilding->levels[nextLevel]) : std::nullopt;
}

unsigned char Player::getTechnologyLevel(unsigned short id) const {
	return id < technologyLevels.size() ? technologyLevels[id] : 0;
}

bool Player::canResearchTechnology(unsigned short levelId) const {
	return std::ranges::any_of(possession->getBuildings(), [this, levelId](const auto* building) {
		return canResearchTechnology(levelId, building);
	});
}

bool Player::canResearchTechnology(unsigned short levelId, const Building* building) const {
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	if (technologyLevels[level->technology] + 1 != level->level) {
		return false;
	}
	if (!std::ranges::any_of(level->unlockAgeIds, [](unsigned short age) { return Game::getWorldAgeController()->hasReachedAge(age); })) {
		return false;
	}
	const auto* technology = Game::getDatabase()->getTechnology(level->technology);
	if (!building->isReady() || !building->getQueue().isEmpty() ||
		!TechnologyUtils::matchesResearchBuilding(technology, building->getDb())) {
		return false;
	}
	if (hasTechnologyResearch(level->technology)) return false;
	const auto effectiveCost = technologyResearchCost(levelId);
	return resources->hasEnough(&effectiveCost);
}

bool Player::hasTechnologyResearch(unsigned short technologyId) const {
	return std::ranges::any_of(possession->getBuildings(), [technologyId](const auto* building) {
		return building->isAlive() && building->getQueue().contains(QueueActionType::TECH_RESEARCH, technologyId);
	});
}

Building* Player::findResearchBuilding(unsigned short levelId) const {
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	const auto* technology = Game::getDatabase()->getTechnology(level->technology);
	if (hasTechnologyResearch(level->technology)) return nullptr;
	for (auto* building : possession->getBuildings()) {
		if (building->isReady() && building->getQueue().isEmpty() &&
			TechnologyUtils::matchesResearchBuilding(technology, building->getDb())) {
			return building;
		}
	}
	return nullptr;
}

float Player::technologyAgeMultiplier(const db_technology_level* level) const {
	const auto* ageController = Game::getWorldAgeController();

	const auto* currentAge = ageController->getCurrentAge();
	unsigned char unlockStage = 0;
	for (const auto ageId : level->unlockAgeIds) {
		if (ageController->hasReachedAge(ageId)) {
			if (const auto* age = ageController->getAge(ageId)) {
				unlockStage = std::max(unlockStage, age->stage);
			}
		}
	}
	const auto difference = currentAge && currentAge->stage >= unlockStage ? currentAge->stage - unlockStage : 0;
	return difference == 0 ? 1.f : difference == 1 ? .8f : difference == 2 ? .6f : difference == 3 ? .4f : .3f;
}

void Player::refreshResourceTechnologyModifiers(const std::vector<const db_technology_effect*>& effects) {
	float foodLostRate = Resources::DEFAULT_FOOD_LOST_RATE;
	float goldGainRate = Resources::DEFAULT_GOLD_GAIN_RATE;
	float stoneRefineBonus = Resources::DEFAULT_STONE_REFINE_BONUS;
	float goldRefineBonus = Resources::DEFAULT_GOLD_REFINE_BONUS;
	float foodStorageMultiplier = Resources::DEFAULT_FOOD_STORAGE_MULTIPLIER;
	float goldStorageMultiplier = Resources::DEFAULT_GOLD_STORAGE_MULTIPLIER;

	const auto appliesToResource = [](const db_technology_effect* effect, ResourceType resource) {
		if (effect->targetKind == TechTargetKind::NONE) return true;
		return effect->targetKind == TechTargetKind::RESOURCE &&
			TechnologyUtils::matchesResourceTarget(effect, cast(resource));
	};

	const auto appliesTo = [&appliesToResource](const auto* effect, TechStat stat, ResourceType resource) {
		return effect->sourceKind == TechSourceKind::PLAYER && effect->stat == stat &&
			appliesToResource(effect, resource);
	};
	foodLostRate = TechnologyUtils::applyEffects(foodLostRate, effects, [&](const auto* effect) { return appliesTo(effect, TechStat::FOOD_DECAY, ResourceType::FOOD); });
	goldGainRate = TechnologyUtils::applyEffects(goldGainRate, effects, [&](const auto* effect) { return appliesTo(effect, TechStat::GOLD_INTEREST, ResourceType::GOLD); });
	stoneRefineBonus = TechnologyUtils::applyEffects(stoneRefineBonus, effects, [&](const auto* effect) { return appliesTo(effect, TechStat::STONE_REFINEMENT, ResourceType::STONE); });
	goldRefineBonus = TechnologyUtils::applyEffects(goldRefineBonus, effects, [&](const auto* effect) { return appliesTo(effect, TechStat::GOLD_REFINEMENT, ResourceType::GOLD); });
	foodStorageMultiplier = TechnologyUtils::applyEffects(foodStorageMultiplier, effects, [&](const auto* effect) { return appliesTo(effect, TechStat::FOOD_STORAGE, ResourceType::FOOD); });
	goldStorageMultiplier = TechnologyUtils::applyEffects(goldStorageMultiplier, effects, [&](const auto* effect) { return appliesTo(effect, TechStat::GOLD_STORAGE, ResourceType::GOLD); });

	resources->setTechnologyModifiers(foodLostRate, goldGainRate, stoneRefineBonus, goldRefineBonus, foodStorageMultiplier, goldStorageMultiplier);
	resources->recalculateBuildingState(possession);
}

db_with_cost Player::technologyResearchCost(unsigned short levelId) const {
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	const auto multiplier = technologyAgeMultiplier(level);
	return {
			static_cast<unsigned short>(std::round(level->cost.values[0] * multiplier)),
		static_cast<unsigned short>(std::round(level->cost.values[1] * multiplier)),
		static_cast<unsigned short>(std::round(level->cost.values[2] * multiplier)),
		static_cast<unsigned short>(std::round(level->cost.values[3] * multiplier))};
}

bool Player::completeTechnology(unsigned short levelId) {
	assert(levelId < Game::getDatabase()->getTechnologyLevels().size());
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	if (level->technology >= technologyLevels.size() || technologyLevels[level->technology] + 1 != level->level) {
		return false;
	}
	technologyLevels[level->technology] = level->level;
	refreshEffectiveLevels();
	return true;
}

std::vector<const db_technology_effect*> Player::activeTechnologyEffects() const {
	std::vector<const db_technology_effect*> effects;
	for (const auto* level : Game::getDatabase()->getTechnologyLevels()) {
		if (!level || level->technology >= technologyLevels.size() || technologyLevels[level->technology] < level->level) {
			continue;
		}
		for (const auto* effect : level->effects) effects.push_back(effect);
	}
	std::ranges::sort(effects, [](const auto* left, const auto* right) {
		return TechnologyUtils::effectComesBefore(left->operation, left->id, right->operation, right->id);
	});
	return effects;
}

float Player::applyTechnologyResourceBonus(float bonus, const db_building* source, unsigned char resourceId) const {
	return TechnologyUtils::applyEffects(bonus, activeTechnologyEffectCache, [&](const auto* effect) {
		return effect->stat == TechStat::RESOURCE_BONUS && effect->sourceKind == TechSourceKind::BUILDING &&
			effect->targetKind == TechTargetKind::RESOURCE && TechnologyUtils::matchesBuildingTag(effect->sourceTag, source) &&
			TechnologyUtils::matchesResourceTarget(effect, resourceId);
	});
}

float Player::applyTechnologyResourceBonus(float bonus, unsigned char resourceId) const {
	return TechnologyUtils::applyEffects(bonus, activeTechnologyEffectCache, [&](const auto* effect) {
		return effect->stat == TechStat::RESOURCE_BONUS && effect->sourceKind == TechSourceKind::RESOURCE &&
			effect->targetKind == TechTargetKind::RESOURCE && TechnologyUtils::matchesResourceTarget(effect, resourceId);
	});
}

bool Player::unitInPlayerNation(const db_unit* unit) {
	return std::ranges::find(unit->nations, dbNation) != unit->nations.end();
}

bool Player::buildingInPlayerNation(const db_building* building) {
	return std::ranges::find(building->nations, dbNation) != building->nations.end();
}

void Player::refreshEffectiveLevels() {
	const auto database = Game::getDatabase();
	activeTechnologyEffectCache = activeTechnologyEffects();
	const auto& effects = activeTechnologyEffectCache;
	auto &allUnits = database->getUnits();

	for (const auto* unit : allUnits) {
		if (!unit) continue;
		auto& level = unitLevels[unit->id];
		if (!unitInPlayerNation(unit)) {
			level.reset();
			continue;
		}
		auto effective = new db_unit_level(*unit->levels[level.level]);

		level.set(unit->id, effective);

		const auto baseAttack = unit->levels[level.level]->attack;
		effective->attack = TechnologyUtils::applyUnitTechStat(baseAttack, effects, unit, TechStat::ATTACK);
		effective->armor = TechnologyUtils::applyUnitTechStat(effective->armor, effects, unit, TechStat::ARMOR);
		effective->maxHp = static_cast<unsigned short>(std::max(1.f, TechnologyUtils::applyUnitTechStat(effective->maxHp, effects, unit, TechStat::MAX_HP)));
		effective->maxSpeed = TechnologyUtils::applyUnitTechStat(effective->maxSpeed, effects, unit, TechStat::SPEED);
		effective->sightRadius = TechnologyUtils::applyUnitTechStat(effective->sightRadius, effects, unit, TechStat::SIGHT_RANGE);
		effective->attackRange = static_cast<short>(TechnologyUtils::applyUnitTechStat(effective->attackRange, effects, unit, TechStat::ATTACK_RANGE));
		effective->attackReload = std::max<short>(1, static_cast<short>(TechnologyUtils::applyUnitTechStat(effective->attackReload, effects, unit, TechStat::ATTACK_RELOAD)));
		effective->collect = TechnologyUtils::applyUnitTechStat(effective->collect, effects, unit, TechStat::GATHER_RATE);
		effective->bonusInfantry = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusInfantry, effects, unit, TechTargetKind::UNIT, TechSourceTag::INFANTRY);
		effective->bonusRange = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusRange, effects, unit, TechTargetKind::UNIT, TechSourceTag::RANGED);
		effective->bonusCavalry = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusCavalry, effects, unit, TechTargetKind::UNIT, TechSourceTag::CAVALRY);
		effective->bonusWorker = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusWorker, effects, unit, TechTargetKind::UNIT, TechSourceTag::WORKER);
		effective->bonusSpecial = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusSpecial, effects, unit, TechTargetKind::UNIT, TechSourceTag::SPECIAL);
		effective->bonusMelee = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusMelee, effects, unit, TechTargetKind::UNIT, TechSourceTag::MELEE);
		effective->bonusHeavy = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusHeavy, effects, unit, TechTargetKind::UNIT, TechSourceTag::HEAVY);
		effective->bonusLight = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusLight, effects, unit, TechTargetKind::UNIT, TechSourceTag::LIGHT);
		// Building-sourced attack bonuses against target categories need effective fields on db_building_attack if introduced.
		// Building-sourced target-specific attack bonuses need effective target-bonus fields on db_building_attack if introduced.
		effective->bonusBuilding = TechnologyUtils::applyUnitAttackTargetBonus(effective->attack, effective->bonusBuilding, effects, unit, TechTargetKind::BUILDING, TechSourceTag::ANY);
		effective->invMaxHp = 1.f / effective->maxHp;
		effective->sqSightRadius = effective->sightRadius * effective->sightRadius;
		effective->interestRange = effective->sightRadius * 0.8f;
		effective->sqInterestRange = effective->interestRange * effective->interestRange;
		effective->sqAttackRange = static_cast<float>(effective->attackRange) * effective->attackRange;
		effective->sqMinSpeed = effective->minSpeed * effective->minSpeed;
		effective->finish(const_cast<db_unit*>(unit));
	}

	for (const auto* building : database->getBuildings()) {
		if (!building) continue;
		auto& playerLevel = buildingLevels[building->id];
		const bool available = buildingInPlayerNation(building);
		if (!available) {
			playerLevel.reset();
			continue;
		}

		playerLevel.set(building->id, new db_building_level(*building->levels[playerLevel.level]));
		// Building attackers currently lack target-category bonus fields; revisit if target-specific building attacks are added.
		// Building attackers currently have no target-category bonus fields; revisit if target-specific building attacks are added.
		// Building-sourced attack-vs-target bonuses need effective target-bonus fields on db_building_attack if introduced.
		playerLevel.effective->attack = TechnologyUtils::applyBuildingTechStat(playerLevel.effective->attack, effects,
			building, TechStat::ATTACK);
		playerLevel.effective->armor = TechnologyUtils::applyBuildingTechStat(playerLevel.effective->armor, effects,
			building, TechStat::ARMOR);
		playerLevel.effective->maxHp = static_cast<unsigned short>(std::max(1.f, TechnologyUtils::applyBuildingTechStat(
			playerLevel.effective->maxHp, effects, building, TechStat::MAX_HP)));
		playerLevel.effective->sightRadius = TechnologyUtils::applyBuildingTechStat(playerLevel.effective->sightRadius,
			effects, building, TechStat::SIGHT_RANGE);
		playerLevel.effective->attackRange = static_cast<short>(TechnologyUtils::applyBuildingTechStat(
			playerLevel.effective->attackRange, effects, building, TechStat::ATTACK_RANGE));
		playerLevel.effective->attackReload = std::max<short>(1, static_cast<short>(TechnologyUtils::applyBuildingTechStat(
			playerLevel.effective->attackReload, effects, building, TechStat::ATTACK_RELOAD)));
		playerLevel.effective->resourceRange = TechnologyUtils::applyBuildingTechStat(playerLevel.effective->resourceRange,
			effects, building, TechStat::RESOURCE_RANGE);
		playerLevel.effective->collect = TechnologyUtils::applyBuildingTechStat(playerLevel.effective->collect, effects,
			building, TechStat::RESOURCE_BONUS);
		playerLevel.effective->invMaxHp = 1.f / playerLevel.effective->maxHp;
		playerLevel.effective->sqSightRadius = playerLevel.effective->sightRadius * playerLevel.effective->sightRadius;
		playerLevel.effective->interestRange = playerLevel.effective->sightRadius * 0.8f;
		playerLevel.effective->sqInterestRange = playerLevel.effective->interestRange * playerLevel.effective->interestRange;
		playerLevel.effective->sqAttackRange = static_cast<float>(playerLevel.effective->attackRange) * playerLevel.effective->attackRange;
		playerLevel.effective->finish(const_cast<db_building*>(building));
	}
	refreshResourceTechnologyModifiers(effects);
}

void Player::addKilled(Physical* physical) const { possession->addKilled(physical); }

void Player::resetScore() { score = -1; }

void Player::restoreUnitLevel(unsigned short id, unsigned char level) {
	if (id < unitLevels.size() && unitLevels[id].id >= 0) {
		unitLevels[id].level = level;
	}
}

void Player::restoreBuildingLevel(unsigned short id, unsigned char level) {
	if (id < buildingLevels.size() && buildingLevels[id].id >= 0) {
		buildingLevels[id].level = level;
	}
}

void Player::restoreTechnologyLevel(unsigned short id, unsigned char level) {
	if (id < technologyLevels.size()) {
		technologyLevels[id] = level;
	}
}

void Player::updateResource1s() const { resources->update1s(possession); }

void Player::updateResourceMonth() const { resources->updateMonth(); }

void Player::updateResourceYear() const { resources->updateYear(); }

void Player::updatePossession() { possession->updateAndClean(resources); }

void Player::add(Unit* unit) const { possession->add(unit); }

void Player::add(Building* building) const { possession->add(building); }

int Player::getScore() {
	if (score < 0) {
		const float visibilityPercent = Game::getEnvironment()->getVisibilityScore(id);
		score = possession->getScore() + visibilityPercent * 1000.f;
	}
	return score;
}

int Player::getWorkersNumber() const { return possession->getWorkersNumber(); }

QueueElement* Player::updateQueue() { return queue.update(); }

void Player::aiAction() { aiOrchestrator.action(); }

void Player::aiOrder() { aiOrchestrator.order(); }
