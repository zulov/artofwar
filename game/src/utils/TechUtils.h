#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "database/db_struct.h"
#include "database/db_technology_struct.h"

namespace TechUtils {

inline bool matchesUnitTag(TechSourceTag tag, const db_unit* unit) {
	switch (tag) {
	case TechSourceTag::ANY: return true;
	case TechSourceTag::ARMY: return !unit->typeWorker;
	case TechSourceTag::WORKER: return unit->typeWorker;
	case TechSourceTag::INFANTRY: return unit->typeInfantry;
	case TechSourceTag::RANGED: return unit->typeRange;
	case TechSourceTag::CAVALRY: return unit->typeCavalry;
	case TechSourceTag::MELEE: return unit->typeMelee;
	case TechSourceTag::HEAVY: return unit->typeHeavy;
	case TechSourceTag::LIGHT: return unit->typeLight;
	case TechSourceTag::SPECIAL: return unit->typeSpecial;
	default: return false;
	}
}

inline bool isSupplyBuilding(bool typeHome, bool typeCenter) { return typeHome || typeCenter; }

inline bool matchesBuildingTag(TechSourceTag tag, const db_building* building) {
	return tag == TechSourceTag::ANY || tag == TechSourceTag::ALL_BUILDINGS ||
		(tag == TechSourceTag::RESOURCE_BUILDING && building->isResourceBuilding()) ||
		(tag == TechSourceTag::DEFENSIVE_BUILDING && building->typeDefence) ||
		(tag == TechSourceTag::TECH_BUILDING && building->parentType[static_cast<int>(ParentBuildingType::TECH)]) ||
		(tag == TechSourceTag::SUPPLY_BUILDING && isSupplyBuilding(building->typeHome, building->typeCenter));
}

inline bool matchesUnitTargetTag(TechSourceTag effectTag, TechSourceTag matchupTag) {
	return effectTag == TechSourceTag::ANY || effectTag == matchupTag;
}

class UnitAttackTargetEffectPredicate {
public:
	UnitAttackTargetEffectPredicate(const db_unit* source, TechTargetKind targetKind, TechSourceTag targetTag)
		: source(source), targetKind(targetKind), targetTag(targetTag) {}

	bool operator()(const db_technology_effect* effect) const {
		if (effect->stat != TechStat::ATTACK || effect->sourceKind != TechSourceKind::UNIT ||
			!matchesUnitTag(effect->sourceTag, source) || effect->targetKind != targetKind) {
			return false;
		}
		if (targetKind == TechTargetKind::UNIT) return matchesUnitTargetTag(effect->targetTag, targetTag);
		if (targetKind == TechTargetKind::BUILDING) {
			return effect->targetTag == TechSourceTag::ANY || effect->targetTag == TechSourceTag::ALL_BUILDINGS;
		}
		return false;
	}

private:
	const db_unit* source;
	TechTargetKind targetKind;
	TechSourceTag targetTag;
};

class UnitStatEffectPredicate {
public:
	UnitStatEffectPredicate(const db_unit* unit, TechStat stat) : unit(unit), stat(stat) {}

	bool operator()(const db_technology_effect* effect) const {
		return effect->sourceKind == TechSourceKind::UNIT && effect->targetKind == TechTargetKind::NONE &&
			effect->stat == stat && matchesUnitTag(effect->sourceTag, unit);
	}

private:
	const db_unit* unit;
	TechStat stat;
};

class BuildingStatEffectPredicate {
public:
	BuildingStatEffectPredicate(const db_building* building, TechStat stat) : building(building), stat(stat) {}

	bool operator()(const db_technology_effect* effect) const {
		return effect->sourceKind == TechSourceKind::BUILDING && effect->targetKind == TechTargetKind::NONE &&
			effect->stat == stat && matchesBuildingTag(effect->sourceTag, building);
	}

private:
	const db_building* building;
	TechStat stat;
};

inline bool matchesResearchBuilding(const db_technology* technology, const db_building* building) {
	return technology && building && std::ranges::find(technology->researchBuildingIds, building->id) !=
		technology->researchBuildingIds.end();
}

inline bool matchesResourceTarget(const db_technology_effect* effect, unsigned char resourceId) {
	if (effect->targetKind != TechTargetKind::RESOURCE) return false;
	switch (effect->resourceType) {
	case TechResourceType::NONE:
	case TechResourceType::ANY: return true;
	case TechResourceType::FOOD: return resourceId == cast(ResourceType::FOOD);
	case TechResourceType::WOOD: return resourceId == cast(ResourceType::WOOD);
	case TechResourceType::STONE: return resourceId == cast(ResourceType::STONE);
	case TechResourceType::GOLD: return resourceId == cast(ResourceType::GOLD);
	default: return false;
	}
}

inline unsigned char operationOrder(TechOperation operation) {
	return operation == TechOperation::ADD ? 0 : 1;
}

inline bool effectComesBefore(TechOperation leftOperation, unsigned short leftId,
		TechOperation rightOperation, unsigned short rightId) {
	const auto leftOrder = operationOrder(leftOperation);
	const auto rightOrder = operationOrder(rightOperation);
	return leftOrder != rightOrder ? leftOrder < rightOrder : leftId < rightId;
}

struct EffectOrder {
	template <typename Effect>
	bool operator()(const Effect* left, const Effect* right) const {
		return effectComesBefore(left->operation, left->id, right->operation, right->id);
	}
};

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
	std::ranges::sort(ordered, EffectOrder{});
	for (const auto* effect : ordered) {
		if (effect->operation == TechOperation::ADD) additive += effect->value;
		else multiplier *= 1.0 + effect->value;
	}
	return static_cast<float>((static_cast<double>(value) + additive) * multiplier);
}

template <typename EffectContainer, typename Predicate>
inline float applyEffectsToAttackBonus(float baseAttack, float effectiveAttack, float bonus,
		const EffectContainer& effects, Predicate&& applies) {
	if (effectiveAttack <= 0.f) return bonus;
	const auto adjustedAttack = applyEffects(baseAttack * (1.f + bonus), effects, std::forward<Predicate>(applies));
	return adjustedAttack / effectiveAttack - 1.f;
}

template <typename EffectContainer>
inline float applyUnitAttackTargetBonus(float effectiveAttack, float bonus, const EffectContainer& effects,
		const db_unit* source, TechTargetKind targetKind, TechSourceTag targetTag) {
	return applyEffectsToAttackBonus(effectiveAttack, effectiveAttack, bonus, effects,
		UnitAttackTargetEffectPredicate(source, targetKind, targetTag));
}

template <typename EffectContainer>
inline float applyUnitTechStat(float value, const EffectContainer& effects, const db_unit* unit, TechStat stat) {
	return applyEffects(value, effects, UnitStatEffectPredicate(unit, stat));
}

template <typename EffectContainer>
inline float applyBuildingTechStat(float value, const EffectContainer& effects, const db_building* building, TechStat stat) {
	return applyEffects(value, effects, BuildingStatEffectPredicate(building, stat));
}

}
