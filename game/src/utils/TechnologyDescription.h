#pragma once

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include <Urho3D/Container/Str.h>
#include <Urho3D/Resource/Localization.h>

#include "Game.h"
#include "database/DatabaseCache.h"
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

inline std::string formatEffectValue(TechnologyOperation operation, float value) {
	if (operation == TechnologyOperation::PERCENT) value *= 100.f;

	const auto number = formatNumber(value);
	const auto signedNumber = value > 0.f ? "+" + number : number;
	return signedNumber + (operation == TechnologyOperation::PERCENT ? "%" : "");
}

inline Urho3D::String localize(const char* key) {
	return Game::getLocalization()->Get(Urho3D::String(key));
}

inline const char* statKey(TechnologyStat stat) {
	switch (stat) {
	case TechnologyStat::ATTACK: return "tech_stat_attack";
	case TechnologyStat::ARMOR: return "tech_stat_armor";
	case TechnologyStat::MAX_HP: return "tech_stat_max_hp";
	case TechnologyStat::SPEED: return "tech_stat_speed";
	case TechnologyStat::SIGHT_RANGE: return "tech_stat_sight_range";
	case TechnologyStat::ATTACK_RANGE: return "tech_stat_attack_range";
	case TechnologyStat::ATTACK_RELOAD: return "tech_stat_attack_reload";
	case TechnologyStat::GATHER_RATE: return "tech_stat_gather_rate";
	case TechnologyStat::RESOURCE_BONUS: return "tech_stat_resource_bonus";
	case TechnologyStat::RESOURCE_RANGE: return "tech_stat_resource_range";
	case TechnologyStat::FOOD_STORAGE: return "tech_stat_food_storage";
	case TechnologyStat::GOLD_STORAGE: return "tech_stat_gold_storage";
	case TechnologyStat::STONE_REFINEMENT: return "tech_stat_stone_refinement";
	case TechnologyStat::GOLD_REFINEMENT: return "tech_stat_gold_refinement";
	case TechnologyStat::BUILD_TIME: return "tech_stat_build_time";
	case TechnologyStat::TRAIN_TIME: return "tech_stat_train_time";
	default: return "tech_stat_unknown";
	}
}

inline const char* scopeKey(TechnologySourceKind kind, TechnologySourceTag tag) {
	switch (kind) {
	case TechnologySourceKind::UNIT:
		switch (tag) {
		case TechnologySourceTag::ANY: return "tech_scope_unit_any";
		case TechnologySourceTag::ARMY: return "tech_scope_unit_army";
		case TechnologySourceTag::WORKER: return "tech_scope_unit_worker";
		case TechnologySourceTag::INFANTRY: return "tech_scope_unit_infantry";
		case TechnologySourceTag::RANGED: return "tech_scope_unit_ranged";
		case TechnologySourceTag::CAVALRY: return "tech_scope_unit_cavalry";
		case TechnologySourceTag::MELEE: return "tech_scope_unit_melee";
		case TechnologySourceTag::HEAVY: return "tech_scope_unit_heavy";
		case TechnologySourceTag::LIGHT: return "tech_scope_unit_light";
		case TechnologySourceTag::SPECIAL: return "tech_scope_unit_special";
		default: return "tech_scope_unit_any";
		}
	case TechnologySourceKind::BUILDING:
		switch (tag) {
		case TechnologySourceTag::ANY:
		case TechnologySourceTag::ALL_BUILDINGS: return "tech_scope_building_any";
		case TechnologySourceTag::RESOURCE_BUILDING: return "tech_scope_building_resource";
		case TechnologySourceTag::DEFENSIVE_BUILDING: return "tech_scope_building_defensive";
		case TechnologySourceTag::TECH_BUILDING: return "tech_scope_building_tech";
		default: return "tech_scope_building_any";
		}
	case TechnologySourceKind::RESOURCE: return "tech_scope_resource_any";
	case TechnologySourceKind::PLAYER: return "tech_scope_player";
	default: return "tech_scope_unknown";
	}
}

inline Urho3D::String scopeName(TechnologySourceKind kind, TechnologySourceTag tag) {
	return localize(scopeKey(kind, tag));
}

inline Urho3D::String targetScopeName(TechnologyTargetKind kind, TechnologySourceTag tag) {
	switch (kind) {
	case TechnologyTargetKind::UNIT: return scopeName(TechnologySourceKind::UNIT, tag);
	case TechnologyTargetKind::BUILDING: return scopeName(TechnologySourceKind::BUILDING, tag);
	case TechnologyTargetKind::RESOURCE: return scopeName(TechnologySourceKind::RESOURCE, tag);
	default: return localize("tech_scope_unknown");
	}
}

inline Urho3D::String resourceName(const std::string& resourceType) {
	const auto name = lowerTechnologyName(resourceType.c_str());
	if (name == "food") return localize("tech_resource_food");
	if (name == "wood") return localize("tech_resource_wood");
	if (name == "stone") return localize("tech_resource_stone");
	if (name == "gold") return localize("tech_resource_gold");
	return {};
}

inline Urho3D::String targetName(const db_technology_effect* effect) {
	if (effect->targetKind == TechnologyTargetKind::RESOURCE) {
		if (const auto name = resourceName(effect->resourceType); !name.Empty()) return name;
		if (lowerTechnologyName(effect->resourceType.c_str()) == "any") return localize("tech_scope_resource_any");
	}

	if (effect->targetId >= 0) {
		auto* database = Game::getDatabase();
		switch (effect->targetKind) {
		case TechnologyTargetKind::UNIT:
			if (static_cast<size_t>(effect->targetId) < database->getUnits().size()) {
				if (const auto* unit = database->getUnit(static_cast<unsigned short>(effect->targetId))) return unit->name;
			}
			break;
		case TechnologyTargetKind::BUILDING:
			if (static_cast<size_t>(effect->targetId) < database->getBuildings().size()) {
				if (const auto* building = database->getBuilding(static_cast<unsigned short>(effect->targetId))) return building->name;
			}
			break;
		case TechnologyTargetKind::RESOURCE:
			if (static_cast<size_t>(effect->targetId) < database->getResourcesSize()) {
				if (const auto* resource = database->getResource(static_cast<unsigned short>(effect->targetId))) return resource->name;
			}
			break;
		default: break;
		}
	}

	return targetScopeName(effect->targetKind, effect->targetTag);
}

inline Urho3D::String effect(const db_technology_effect* technologyEffect) {
	if (!technologyEffect) return {};

	Urho3D::String result = Urho3D::String(formatEffectValue(technologyEffect->operation, technologyEffect->value).c_str())
		+ " " + localize(statKey(technologyEffect->stat)) + " "
		+ localize("tech_effect_for") + " "
		+ scopeName(technologyEffect->sourceKind, technologyEffect->sourceTag);

	if (technologyEffect->targetKind != TechnologyTargetKind::NONE) {
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
