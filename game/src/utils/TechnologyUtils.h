#pragma once

#include <algorithm>
#include <vector>

#include "database/db_struct.h"
#include "database/db_technology_struct.h"
#include "player/Player.h"

namespace TechnologyUtils {

inline bool matchesUnitTag(TechnologySourceTag tag, const db_unit* unit) {
	switch (tag) {
	case TechnologySourceTag::ANY: return true;
	case TechnologySourceTag::ARMY: return !unit->typeWorker;
	case TechnologySourceTag::WORKER: return unit->typeWorker;
	case TechnologySourceTag::INFANTRY: return unit->typeInfantry;
	case TechnologySourceTag::RANGED: return unit->typeRange;
	case TechnologySourceTag::CAVALRY: return unit->typeCavalry;
	case TechnologySourceTag::MELEE: return unit->typeMelee;
	case TechnologySourceTag::HEAVY: return unit->typeHeavy;
	case TechnologySourceTag::LIGHT: return unit->typeLight;
	case TechnologySourceTag::SPECIAL: return unit->typeSpecial;
	default: return false;
	}
}

inline bool matchesBuildingTag(TechnologySourceTag tag, const db_building* building) {
	return tag == TechnologySourceTag::ANY || tag == TechnologySourceTag::ALL_BUILDINGS ||
		(tag == TechnologySourceTag::RESOURCE_BUILDING && building->isResourceBuilding()) ||
		(tag == TechnologySourceTag::DEFENSIVE_BUILDING && building->typeDefence) ||
		(tag == TechnologySourceTag::TECH_BUILDING && building->parentType[static_cast<int>(ParentBuildingType::TECH)]);
}

inline bool matchesResearchBuilding(const db_technology* technology, const db_building* building) {
	return technology && building && std::ranges::find(technology->researchBuildingIds, building->id) !=
		technology->researchBuildingIds.end();
}

inline bool matchesTargetId(short targetId, unsigned short id) {
	return targetId < 0 || static_cast<unsigned short>(targetId) == id;
}

inline bool matchesAttackTarget(const db_technology_effect* effect, const db_unit* target) {
	return effect->targetKind == TechnologyTargetKind::UNIT && matchesTargetId(effect->targetId, target->id) &&
		matchesUnitTag(effect->targetTag, target);
}

inline bool matchesAttackTarget(const db_technology_effect* effect, const db_building* target) {
	return effect->targetKind == TechnologyTargetKind::BUILDING && matchesTargetId(effect->targetId, target->id) &&
		matchesBuildingTag(effect->targetTag, target);
}

inline bool matchesResourceTarget(const db_technology_effect* effect, unsigned char resourceId) {
	if (effect->targetKind != TechnologyTargetKind::RESOURCE || !matchesTargetId(effect->targetId, resourceId)) {
		return false;
	}
	switch (effect->resourceType) {
	case TechnologyResourceType::NONE:
	case TechnologyResourceType::ANY: return true;
	case TechnologyResourceType::FOOD: return resourceId == cast(ResourceType::FOOD);
	case TechnologyResourceType::WOOD: return resourceId == cast(ResourceType::WOOD);
	case TechnologyResourceType::STONE: return resourceId == cast(ResourceType::STONE);
	case TechnologyResourceType::GOLD: return resourceId == cast(ResourceType::GOLD);
	default: return false;
	}
}

inline bool isActive(const Player* player, const db_technology_level* technology) {
	return technology && technology->technology < player->getTechnologyLevels().size() &&
		player->getTechnologyLevel(technology->technology) >= technology->level;
}

inline unsigned char operationOrder(TechnologyOperation operation) {
	return operation == TechnologyOperation::ADD ? 0 : 1;
}

inline bool effectComesBefore(TechnologyOperation leftOperation, unsigned short leftId,
		TechnologyOperation rightOperation, unsigned short rightId) {
	const auto leftOrder = operationOrder(leftOperation);
	const auto rightOrder = operationOrder(rightOperation);
	return leftOrder != rightOrder ? leftOrder < rightOrder : leftId < rightId;
}

inline std::vector<const db_technology_effect*> activeEffects(const Player* player,
		const std::vector<db_technology_level*>& levels) {
	std::vector<const db_technology_effect*> result;
	for (const auto* level : levels) {
		if (!isActive(player, level)) continue;
		for (const auto* effect : level->effects) result.push_back(effect);
	}
	std::ranges::sort(result, [](const auto* left, const auto* right) {
		return effectComesBefore(left->operation, left->id, right->operation, right->id);
	});
	return result;
}

template <typename Effect>
inline void applyEffect(float& value, const Effect* effect) {
	value = effect->operation == TechnologyOperation::PERCENT ? value * (1.f + effect->value) : value + effect->value;
}

template <typename EffectContainer, typename Predicate>
inline float applyEffects(float value, const EffectContainer& effects, Predicate&& applies) {
	using EffectPointer = typename EffectContainer::value_type;
	std::vector<EffectPointer> ordered;
	double additive = 0.0;
	double multiplier = 1.0;
	ordered.reserve(effects.size());
	for (const auto* effect : effects) {
		if (effect && applies(effect)) ordered.push_back(effect);
	}
	std::ranges::sort(ordered, [](const auto* left, const auto* right) {
		return effectComesBefore(left->operation, left->id, right->operation, right->id);
	});
	for (const auto* effect : ordered) {
		if (effect->operation == TechnologyOperation::ADD) additive += effect->value;
		else multiplier *= 1.0 + effect->value;
	}
	return static_cast<float>((static_cast<double>(value) + additive) * multiplier);
}

}
