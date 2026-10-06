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
#include "objects/queue/QueueElement.h"
#include "utils/TechnologyUtils.h"

Player::Player(unsigned char nationId, unsigned char team, unsigned char id, unsigned char color, Urho3D::String name,
			   bool active, unsigned currentBuildingUId, unsigned currentUnitUId) :
	team(team), id(id), active(active), color(color), currentBuildingUId(currentBuildingUId),
	currentUnitUId(currentUnitUId), dbNation(Game::getDatabase()->getNation(nationId)),
	possession(new Possession(nationId)), resources(new Resources()), aiOrchestrator(this, dbNation, &aiHistory),
	name(std::move(name)) {
	technologyLevels.assign(Game::getDatabase()->getTechnologies().size(), 0);
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

char Player::upgradeLevel(QueueActionType type, int id) {
	switch (type) {
	case QueueActionType::UNIT_LEVEL:
		if (id >= 0 && id < unitLevels.size() && unitLevels[id].id >= 0) {
			if (const auto next = getNextUnitLevel(id);
				next && Game::getWorldAgeController()->isLevelAvailable((*next)->ageStage)) {
				++unitLevels[id].level;
				refreshEffectiveLevels();
				return unitLevels[id].level;
			}
		}
		break;
	case QueueActionType::BUILDING_LEVEL:
		if (id >= 0 && id < buildingLevels.size() && buildingLevels[id].id >= 0) {
			if (const auto next = getNextBuildingLevel(id);
				next && Game::getWorldAgeController()->isLevelAvailable((*next)->ageStage)) {
				++buildingLevels[id].level;
				refreshEffectiveLevels();
				return buildingLevels[id].level;
			}
		}
		break;
	case QueueActionType::UNIT_UPGRADE:
		break;
	default:;
	}
	return -1;
}

unsigned char Player::getNation() const { return dbNation->id; }

db_unit_level* Player::getUnitLevel(unsigned short id) const {
	return id < unitLevels.size() ? unitLevels[id].effective : nullptr;
}

bool Player::startTechnologyResearch(unsigned short levelId) {
	if (!canResearchTechnology(levelId)) return false;
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	const auto effectiveCost = technologyResearchCost(levelId);
	if (!resources->reduce(&effectiveCost)) return false;
	const auto duration = technologyResearchDuration(levelId);
	queue.add(QueueActionType::TECH_RESEARCH, level->technology, levelId, 1, duration);
	return true;
}

unsigned short Player::technologyResearchDuration(unsigned short levelId) const {
	if (levelId >= Game::getDatabase()->getTechnologyLevels().size()) return 1;
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	return static_cast<unsigned short>(std::max(1.f, std::round(level->researchTime * technologyAgeMultiplier(level))));
}

db_building_level* Player::getBuildingLevel(unsigned short id) const {
	return id < buildingLevels.size() ? buildingLevels[id].effective : nullptr;
}

std::optional<db_unit_level*> Player::getNextUnitLevel(unsigned short id) const {
	if (id >= unitLevels.size() || unitLevels[id].id < 0) return std::nullopt;
	const auto* unit = Game::getDatabase()->getUnit(id);
	const auto nextLevel = static_cast<size_t>(unitLevels[id].level) + 1;
	return unit && nextLevel < unit->levels.size() ? std::optional(unit->levels[nextLevel]) : std::nullopt;
}

std::optional<db_building_level*> Player::getNextBuildingLevel(unsigned short id) const {
	if (id >= buildingLevels.size() || buildingLevels[id].id < 0) return std::nullopt;
	const auto* building = Game::getDatabase()->getBuilding(id);
	const auto nextLevel = static_cast<size_t>(buildingLevels[id].level) + 1;
	return building && nextLevel < building->levels.size() ? std::optional(building->levels[nextLevel]) : std::nullopt;
}

unsigned char Player::getTechnologyLevel(unsigned short id) const {
	return id < technologyLevels.size() ? technologyLevels[id] : 0;
}

bool Player::canResearchTechnology(unsigned short levelId) const {
	if (levelId >= Game::getDatabase()->getTechnologyLevels().size()) return false;
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	if (level->technology >= technologyLevels.size() || technologyLevels[level->technology] + 1 != level->level) {
		return false;
	}
	if (Game::getWorldAgeController() == nullptr ||
		!std::ranges::any_of(level->unlockAgeIds, [](unsigned short age) {
			return Game::getWorldAgeController()->hasReachedAge(age);
		})) {
		return false;
	}
	const auto* technology = Game::getDatabase()->getTechnology(level->technology);
	if (technology && !technology->researchBuilding.empty() && technology->researchBuilding != "none") {
		const auto& owned = possession->getBuildings();
		const bool hasBuilding = std::ranges::any_of(owned, [&technology](const auto* building) {
			if (!building->isReady()) return false;
			return (technology->researchBuilding == "blacksmith" && building->getDb()->typeTechBlacksmith) ||
				(technology->researchBuilding == "university" && building->getDb()->typeTechUniversity);
		});
		if (!hasBuilding) return false;
	}
	const auto effectiveCost = technologyResearchCost(levelId);
	return resources->hasEnough(&effectiveCost) && queue.isEmpty();
}

float Player::technologyAgeMultiplier(const db_technology_level* level) const {
	const auto* ageController = Game::getWorldAgeController();
	if (!ageController) return 1.f;

	const auto* currentAge = ageController->getAge(ageController->getCurrentAgeId());
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

db_with_cost Player::technologyResearchCost(unsigned short levelId) const {
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	const auto multiplier = technologyAgeMultiplier(level);
	return db_with_cost(
		static_cast<unsigned short>(std::round(level->cost.values[0] * multiplier)),
		static_cast<unsigned short>(std::round(level->cost.values[1] * multiplier)),
		static_cast<unsigned short>(std::round(level->cost.values[2] * multiplier)),
		static_cast<unsigned short>(std::round(level->cost.values[3] * multiplier)));
}

bool Player::completeTechnology(unsigned short levelId) {
	if (levelId >= Game::getDatabase()->getTechnologyLevels().size()) return false;
	const auto* level = Game::getDatabase()->getTechnologyLevel(levelId);
	if (level->technology >= technologyLevels.size() || technologyLevels[level->technology] + 1 != level->level) {
		return false;
	}
	technologyLevels[level->technology] = level->level;
	refreshEffectiveLevels();
	return true;
}

float Player::applyTechnologyAttack(float attack, const db_unit* source, const db_unit* target) const {
	for (const auto* technology : Game::getDatabase()->getTechnologyLevels()) {
		if (!TechnologyUtils::isActive(this, technology)) continue;
		for (const auto* effect : technology->effects) {
			if (effect->stat == TechnologyStat::ATTACK &&
				effect->sourceKind == TechnologySourceKind::UNIT && effect->targetKind == TechnologyTargetKind::UNIT &&
				TechnologyUtils::matchesUnitTag(effect->sourceTag, source) && TechnologyUtils::matchesAttackTarget(effect, target)) {
				TechnologyUtils::applyEffect(attack, effect);
			}
		}
	}
	return attack;
}

float Player::applyTechnologyAttack(float attack, const db_unit* source, const db_building* target) const {
	for (const auto* technology : Game::getDatabase()->getTechnologyLevels()) {
		if (!TechnologyUtils::isActive(this, technology)) continue;
		for (const auto* effect : technology->effects) {
			if (effect->stat == TechnologyStat::ATTACK &&
				effect->sourceKind == TechnologySourceKind::UNIT && effect->targetKind == TechnologyTargetKind::BUILDING &&
				TechnologyUtils::matchesUnitTag(effect->sourceTag, source) && TechnologyUtils::matchesAttackTarget(effect, target)) {
				TechnologyUtils::applyEffect(attack, effect);
			}
		}
	}
	return attack;
}

float Player::applyTechnologyAttack(float attack, const db_building* source, const db_unit* target) const {
	for (const auto* technology : Game::getDatabase()->getTechnologyLevels()) {
		if (!TechnologyUtils::isActive(this, technology)) continue;
		for (const auto* effect : technology->effects) {
			if (effect->stat == TechnologyStat::ATTACK &&
				effect->sourceKind == TechnologySourceKind::BUILDING && effect->targetKind == TechnologyTargetKind::UNIT &&
				TechnologyUtils::matchesBuildingTag(effect->sourceTag, source) && TechnologyUtils::matchesAttackTarget(effect, target)) {
				TechnologyUtils::applyEffect(attack, effect);
			}
		}
	}
	return attack;
}

float Player::applyTechnologyAttack(float attack, const db_building* source, const db_building* target) const {
	for (const auto* technology : Game::getDatabase()->getTechnologyLevels()) {
		if (!TechnologyUtils::isActive(this, technology)) continue;
		for (const auto* effect : technology->effects) {
			if (effect->stat == TechnologyStat::ATTACK &&
				effect->sourceKind == TechnologySourceKind::BUILDING && effect->targetKind == TechnologyTargetKind::BUILDING &&
				TechnologyUtils::matchesBuildingTag(effect->sourceTag, source) && TechnologyUtils::matchesAttackTarget(effect, target)) {
				TechnologyUtils::applyEffect(attack, effect);
			}
		}
	}
	return attack;
}

float Player::applyTechnologyResourceBonus(float bonus, const db_building* source, unsigned char resourceId) const {
	for (const auto* technology : Game::getDatabase()->getTechnologyLevels()) {
		if (!TechnologyUtils::isActive(this, technology)) continue;
		for (const auto* effect : technology->effects) {
			if (effect->stat == TechnologyStat::RESOURCE_BONUS &&
				effect->sourceKind == TechnologySourceKind::BUILDING && effect->targetKind == TechnologyTargetKind::RESOURCE &&
				TechnologyUtils::matchesBuildingTag(effect->sourceTag, source) && TechnologyUtils::matchesResourceTarget(effect, resourceId)) {
				TechnologyUtils::applyEffect(bonus, effect);
			}
		}
	}
	return bonus;
}

float Player::applyTechnologyResourceBonus(float bonus, unsigned char resourceId) const {
	for (const auto* technology : Game::getDatabase()->getTechnologyLevels()) {
		if (!TechnologyUtils::isActive(this, technology)) continue;
		for (const auto* effect : technology->effects) {
			if (effect->stat == TechnologyStat::RESOURCE_BONUS &&
				effect->sourceKind == TechnologySourceKind::RESOURCE && effect->targetKind == TechnologyTargetKind::RESOURCE &&
				TechnologyUtils::matchesResourceTarget(effect, resourceId)) {
				TechnologyUtils::applyEffect(bonus, effect);
			}
		}
	}
	return bonus;
}

void Player::refreshEffectiveLevels() {
	const auto& database = *Game::getDatabase();
	if (unitLevels.size() != database.getUnits().size()) {
		unitLevels.clear();
		unitLevels.resize(database.getUnits().size());
	}
	for (const auto* unit : database.getUnits()) {
		if (!unit) continue;
		auto& playerLevel = unitLevels[unit->id];
		const bool available = std::ranges::find(unit->nations, dbNation) != unit->nations.end();
		if (!available || unit->levels.empty()) {
			playerLevel.id = -1;
			playerLevel.level = 0;
			delete playerLevel.effective;
			playerLevel.effective = nullptr;
			continue;
		}
		playerLevel.id = unit->id;
		playerLevel.level = std::min<unsigned char>(playerLevel.level, static_cast<unsigned char>(unit->levels.size() - 1));
		delete playerLevel.effective;
		playerLevel.effective = new db_unit_level(*unit->levels[playerLevel.level]);
		for (const auto* technology : database.getTechnologyLevels()) {
			if (!technology || technology->technology >= technologyLevels.size() || technologyLevels[technology->technology] < technology->level) continue;
			for (const auto* effect : technology->effects) {
				if (effect->sourceKind != TechnologySourceKind::UNIT || effect->targetKind != TechnologyTargetKind::NONE ||
					!TechnologyUtils::matchesUnitTag(effect->sourceTag, unit)) continue;
				switch (effect->stat) {
				case TechnologyStat::ATTACK: TechnologyUtils::applyEffect(playerLevel.effective->attack, effect); break;
				case TechnologyStat::ARMOR: TechnologyUtils::applyEffect(playerLevel.effective->armor, effect); break;
				case TechnologyStat::MAX_HP: { float value = playerLevel.effective->maxHp; TechnologyUtils::applyEffect(value, effect); playerLevel.effective->maxHp = static_cast<unsigned short>(std::max(1.f, value)); break; }
				case TechnologyStat::SPEED: TechnologyUtils::applyEffect(playerLevel.effective->maxSpeed, effect); break;
				case TechnologyStat::SIGHT_RANGE: TechnologyUtils::applyEffect(playerLevel.effective->sightRadius, effect); break;
				case TechnologyStat::ATTACK_RANGE: { float value = playerLevel.effective->attackRange; TechnologyUtils::applyEffect(value, effect); playerLevel.effective->attackRange = static_cast<short>(value); break; }
				case TechnologyStat::ATTACK_RELOAD: { float value = playerLevel.effective->attackReload; TechnologyUtils::applyEffect(value, effect); playerLevel.effective->attackReload = std::max<short>(1, static_cast<short>(value)); break; }
				case TechnologyStat::GATHER_RATE: TechnologyUtils::applyEffect(playerLevel.effective->collect, effect); break;
				default: break;
				}
			}
		}
		playerLevel.effective->invMaxHp = 1.f / playerLevel.effective->maxHp;
		playerLevel.effective->sqSightRadius = playerLevel.effective->sightRadius * playerLevel.effective->sightRadius;
		playerLevel.effective->interestRange = playerLevel.effective->sightRadius * 0.8f;
		playerLevel.effective->sqInterestRange = playerLevel.effective->interestRange * playerLevel.effective->interestRange;
		playerLevel.effective->sqAttackRange = static_cast<float>(playerLevel.effective->attackRange) * playerLevel.effective->attackRange;
		playerLevel.effective->sqMinSpeed = playerLevel.effective->minSpeed * playerLevel.effective->minSpeed;
		playerLevel.effective->finish(const_cast<db_unit*>(unit));
	}

	if (buildingLevels.size() != database.getBuildings().size()) {
		buildingLevels.clear();
		buildingLevels.resize(database.getBuildings().size());
	}
	for (const auto* building : database.getBuildings()) {
		if (!building) continue;
		auto& playerLevel = buildingLevels[building->id];
		const bool available = std::ranges::find(building->nations, dbNation) != building->nations.end();
		if (!available || building->levels.empty()) {
			playerLevel.id = -1;
			playerLevel.level = 0;
			delete playerLevel.effective;
			playerLevel.effective = nullptr;
			continue;
		}
		playerLevel.id = building->id;
		playerLevel.level = std::min<unsigned char>(playerLevel.level, static_cast<unsigned char>(building->levels.size() - 1));
		delete playerLevel.effective;
		playerLevel.effective = new db_building_level(*building->levels[playerLevel.level]);
		for (const auto* technology : database.getTechnologyLevels()) {
			if (!technology || technology->technology >= technologyLevels.size() || technologyLevels[technology->technology] < technology->level) continue;
			for (const auto* effect : technology->effects) {
				if (effect->sourceKind != TechnologySourceKind::BUILDING ||
					effect->targetKind != TechnologyTargetKind::NONE || !TechnologyUtils::matchesBuildingTag(effect->sourceTag, building)) continue;
				switch (effect->stat) {
				case TechnologyStat::ATTACK: TechnologyUtils::applyEffect(playerLevel.effective->attack, effect); break;
				case TechnologyStat::ARMOR: TechnologyUtils::applyEffect(playerLevel.effective->armor, effect); break;
				case TechnologyStat::MAX_HP: { float value = playerLevel.effective->maxHp; TechnologyUtils::applyEffect(value, effect); playerLevel.effective->maxHp = static_cast<unsigned short>(std::max(1.f, value)); break; }
				case TechnologyStat::SIGHT_RANGE: TechnologyUtils::applyEffect(playerLevel.effective->sightRadius, effect); break;
				case TechnologyStat::ATTACK_RANGE: { float value = playerLevel.effective->attackRange; TechnologyUtils::applyEffect(value, effect); playerLevel.effective->attackRange = static_cast<short>(value); break; }
				case TechnologyStat::ATTACK_RELOAD: { float value = playerLevel.effective->attackReload; TechnologyUtils::applyEffect(value, effect); playerLevel.effective->attackReload = std::max<short>(1, static_cast<short>(value)); break; }
				case TechnologyStat::RESOURCE_RANGE: TechnologyUtils::applyEffect(playerLevel.effective->resourceRange, effect); break;
				case TechnologyStat::RESOURCE_BONUS: TechnologyUtils::applyEffect(playerLevel.effective->collect, effect); break;
				default: break;
				}
			}
		}
		playerLevel.effective->invMaxHp = 1.f / playerLevel.effective->maxHp;
		playerLevel.effective->sqSightRadius = playerLevel.effective->sightRadius * playerLevel.effective->sightRadius;
		playerLevel.effective->interestRange = playerLevel.effective->sightRadius * 0.8f;
		playerLevel.effective->sqInterestRange = playerLevel.effective->interestRange * playerLevel.effective->interestRange;
		playerLevel.effective->sqAttackRange = static_cast<float>(playerLevel.effective->attackRange) * playerLevel.effective->attackRange;
		playerLevel.effective->finish(const_cast<db_building*>(building));
	}
}

void Player::addKilled(Physical* physical) const { possession->addKilled(physical); }

void Player::resetScore() { score = -1; }

void Player::restoreUnitLevel(unsigned short id, char level) {
	if (id < unitLevels.size() && unitLevels[id].id >= 0) {
		unitLevels[id].level = static_cast<unsigned char>(std::max(0, static_cast<int>(level)));
	}
}

void Player::restoreBuildingLevel(unsigned short id, char level) {
	if (id < buildingLevels.size() && buildingLevels[id].id >= 0) {
		buildingLevels[id].level = static_cast<unsigned char>(std::max(0, static_cast<int>(level)));
	}
}

void Player::restoreTechnologyLevel(unsigned short id, unsigned char level) {
	if (id < technologyLevels.size()) technologyLevels[id] = level;
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
