#pragma once

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/Localization.h>

#include "Game.h"
#include "database/db_technology_struct.h"

namespace TechnologyDescription {

inline std::string formatNumber(float value) {
	if (std::abs(value) < 0.005f) value = 0.f;

	std::ostringstream stream;
	stream << std::fixed << std::setprecision(2) << value;
	auto result = stream.str();
	while (result.size() > 1 && result.back() == '0') result.pop_back();
	if (!result.empty() && result.back() == '.') result.pop_back();
	return result;
}

inline std::string formatEffectValue(TechOperation operation, float value) {
	if (operation == TechOperation::PERCENT) value *= 100.f;

	const auto number = formatNumber(value);
	const auto signedNumber = value > 0.f ? "+" + number : number;
	return signedNumber + (operation == TechOperation::PERCENT ? "%" : "");
}

inline Urho3D::String localize(const char* key) {
	return Game::getLocalization()->Get(Urho3D::String(key));
}

inline const char* statKey(TechStat stat) {
	switch (stat) {
	case TechStat::ATTACK: return "tech_stat_attack";
	case TechStat::ARMOR: return "tech_stat_armor";
	case TechStat::MAX_HP: return "tech_stat_max_hp";
	case TechStat::SPEED: return "tech_stat_speed";
	case TechStat::SIGHT_RANGE: return "tech_stat_sight_range";
	case TechStat::ATTACK_RANGE: return "tech_stat_attack_range";
	case TechStat::ATTACK_RELOAD: return "tech_stat_attack_reload";
	case TechStat::GATHER_RATE: return "tech_stat_gather_rate";
	case TechStat::RESOURCE_BONUS: return "tech_stat_resource_bonus";
	case TechStat::RESOURCE_RANGE: return "tech_stat_resource_range";
	case TechStat::FOOD_STORAGE: return "tech_stat_food_storage";
	case TechStat::GOLD_STORAGE: return "tech_stat_gold_storage";
	case TechStat::STONE_REFINEMENT: return "tech_stat_stone_refinement";
	case TechStat::GOLD_REFINEMENT: return "tech_stat_gold_refinement";
	case TechStat::BUILD_TIME: return "tech_stat_build_time";
	case TechStat::TRAIN_TIME: return "tech_stat_train_time";
	case TechStat::FOOD_DECAY: return "tech_stat_food_decay";
	case TechStat::GOLD_INTEREST: return "tech_stat_gold_interest";
	default: return "tech_stat_unknown";
	}
}

inline const char* scopeKey(TechSourceKind kind, TechSourceTag tag) {
	switch (kind) {
	case TechSourceKind::UNIT:
		switch (tag) {
		case TechSourceTag::ANY: return "tech_scope_unit_any";
		case TechSourceTag::ARMY: return "tech_scope_unit_army";
		case TechSourceTag::WORKER: return "tech_scope_unit_worker";
		case TechSourceTag::INFANTRY: return "tech_scope_unit_infantry";
		case TechSourceTag::RANGED: return "tech_scope_unit_ranged";
		case TechSourceTag::CAVALRY: return "tech_scope_unit_cavalry";
		case TechSourceTag::MELEE: return "tech_scope_unit_melee";
		case TechSourceTag::HEAVY: return "tech_scope_unit_heavy";
		case TechSourceTag::LIGHT: return "tech_scope_unit_light";
		case TechSourceTag::SPECIAL: return "tech_scope_unit_special";
		default: return "tech_scope_unit_any";
		}
	case TechSourceKind::BUILDING:
		switch (tag) {
		case TechSourceTag::ANY:
		case TechSourceTag::ALL_BUILDINGS: return "tech_scope_building_any";
		case TechSourceTag::RESOURCE_BUILDING: return "tech_scope_building_resource";
		case TechSourceTag::DEFENSIVE_BUILDING: return "tech_scope_building_defensive";
		case TechSourceTag::TECH_BUILDING: return "tech_scope_building_tech";
		default: return "tech_scope_building_any";
		}
	case TechSourceKind::RESOURCE: return "tech_scope_resource_any";
	case TechSourceKind::PLAYER: return "tech_scope_player";
	default: return "tech_scope_unknown";
	}
}

inline Urho3D::String scopeName(TechSourceKind kind, TechSourceTag tag) {
	return localize(scopeKey(kind, tag));
}

inline Urho3D::String targetScopeName(TechTargetKind kind, TechSourceTag tag) {
	switch (kind) {
	case TechTargetKind::UNIT: return scopeName(TechSourceKind::UNIT, tag);
	case TechTargetKind::BUILDING: return scopeName(TechSourceKind::BUILDING, tag);
	case TechTargetKind::RESOURCE: return scopeName(TechSourceKind::RESOURCE, tag);
	default: return localize("tech_scope_unknown");
	}
}

inline Urho3D::String resourceName(TechResourceType resourceType) {
	switch (resourceType) {
	case TechResourceType::FOOD: return localize("resource_food");
	case TechResourceType::WOOD: return localize("resource_wood");
	case TechResourceType::STONE: return localize("resource_stone");
	case TechResourceType::GOLD: return localize("resource_gold");
	default: return {};
	}
}

inline Urho3D::String targetName(const db_technology_effect* effect) {
	if (effect->targetKind == TechTargetKind::RESOURCE) {
		if (const auto name = resourceName(effect->resourceType); !name.Empty()) return name;
		if (effect->resourceType == TechResourceType::ANY) return localize("tech_scope_resource_any");
	}
	return targetScopeName(effect->targetKind, effect->targetTag);
}

inline Urho3D::String effect(const db_technology_effect* technologyEffect) {
	if (!technologyEffect) return {};

	Urho3D::String result = Urho3D::String(formatEffectValue(technologyEffect->operation, technologyEffect->value).c_str())
		+ " " + localize(statKey(technologyEffect->stat)) + " "
		+ localize("tech_effect_for") + " "
		+ scopeName(technologyEffect->sourceKind, technologyEffect->sourceTag);

	if (technologyEffect->targetKind != TechTargetKind::NONE) {
		result += " " + localize("tech_effect_affecting") + " " + targetName(technologyEffect);
	}
	return result;
}

inline Urho3D::String effects(const db_technology_level* level) {
	if (!level || level->effects.empty()) return {};

	Urho3D::String result = localize("tech_effects");
	for (const auto* technologyEffect : level->effects) {
		result += "\n" + effect(technologyEffect);
	}
	return result;
}

} // namespace TechnologyDescription
