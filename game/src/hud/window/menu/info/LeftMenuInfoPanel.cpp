#include "LeftMenuInfoPanel.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Window.h>
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/db_other_struct.h"
#include "database/db_struct.h"
#include "database/db_technology_struct.h"
#include "hud/HudData.h"
#include "hud/UiUtils.h"
#include "objects/ActionType.h"
#include "objects/NamesCache.h"
#include "objects/Physical.h"
#include "objects/unit/order/UnitConst.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "utils/TechnologyDescription.h"
#include "utils/NamesUtils.h"
#include <type_traits>

void LeftMenuInfoPanel::appendStat(Urho3D::String& message, const char* labelKey, const Urho3D::String& value) {
	message += Game::getLocalization()->Get(labelKey) + ": " + value + "\n";
}

void LeftMenuInfoPanel::appendStat(Urho3D::String& message, const char* labelKey, float value) {
	appendStat(message, labelKey, Urho3D::String(asStringF(value, 1).c_str()));
}

template <typename T>
void LeftMenuInfoPanel::appendStatIfPositive(Urho3D::String& message, const char* labelKey, T value) {
	if (value <= 0) {
		return;
	}
	if constexpr (std::is_floating_point_v<T>) {
		appendStat(message, labelKey, value);
	} else {
		appendStat(message, labelKey, Urho3D::String(value));
	}
}

void LeftMenuInfoPanel::appendUnitStats(Urho3D::String& message, const db_unit_level* level, bool upgrade) {
	appendStat(message, "hud_stat_health", Urho3D::String(level->maxHp));
	appendStat(message, "hud_stat_armor", level->armor);
	appendStatIfPositive(message, "hud_stat_attack", level->attack);
	appendStatIfPositive(message, "hud_stat_attack_range", level->attackRange);
	appendStatIfPositive(message, "hud_stat_attack_reload", level->attackReload);
	appendStat(message, "hud_stat_speed", level->maxSpeed);
	appendStat(message, "hud_stat_sight", level->sightRadius);
	appendStatIfPositive(message, "hud_stat_gather_rate", level->collect);
	const auto time = upgrade ? level->upgradeTime : level->buildTime;
	appendStatIfPositive(message, upgrade ? "hud_stat_upgrade_time" : "hud_stat_train_time", time);
}

void LeftMenuInfoPanel::appendBuildingStats(Urho3D::String& message, const db_building* building,
                                           const db_building_level* level, bool upgrade) {
	appendStat(message, "hud_stat_health", Urho3D::String(level->maxHp));
	appendStat(message, "hud_stat_armor", level->armor);
	appendStatIfPositive(message, "hud_stat_attack", level->attack);
	appendStatIfPositive(message, "hud_stat_attack_range", level->attackRange);
	appendStatIfPositive(message, "hud_stat_attack_reload", level->attackReload);
	appendStat(message, "hud_stat_sight", level->sightRadius);
	appendStat(message, "hud_stat_size", Urho3D::String(static_cast<int>(building->size.x_)) + " x " +
	                                       Urho3D::String(static_cast<int>(building->size.y_)));
	appendStatIfPositive(message, "hud_stat_gather_rate", level->collect);
	appendStatIfPositive(message, "hud_stat_resource_range", level->resourceRange);
	appendStatIfPositive(message, "hud_stat_food_storage", level->foodStorage);
	appendStatIfPositive(message, "hud_stat_gold_storage", level->goldStorage);
	appendStatIfPositive(message, "hud_stat_stone_refinement", level->stoneRefineCapacity);
	appendStatIfPositive(message, "hud_stat_gold_refinement", level->goldRefineCapacity);
	appendStatIfPositive(message, "hud_stat_queue_capacity", level->queueMaxCapacity);
	const auto time = upgrade ? level->upgradeTime : level->buildTime;
	appendStatIfPositive(message, upgrade ? "hud_stat_upgrade_time" : "hud_stat_build_time", time);
}

LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : SimplePanel(root, _style, "LeftMenuInfoPanel", {}) {
}

void LeftMenuInfoPanel::createBody() {
	text = addChildText(window, "LeftMenuInfoText", style);
}

void LeftMenuInfoPanel::updateSelected(SelectedInfo* selectedInfo) {
	if (!hoverIsOn) {
		selectedIsOn = true;
		if (selectedInfo->getAllNumber() == 1) {
			auto optInfoTypes = selectedInfo->getOneSelectedTypeInfo();
			if (optInfoTypes.has_value()) {
				text->SetText(optInfoTypes.value()->getData().at(0)->getInfo());
				setVisible(true);
			}
		} else if (selectedInfo->isSthSelected()) {
			Urho3D::String msg = "";
			for (const auto& selectedType : selectedInfo->getSelectedTypes()) {
				auto& data = selectedType->getData();

				if (!data.empty()) {
					float sumHpPer = 0.f;
					for (const auto physical : data) {
						sumHpPer += physical->getHealthPercent();
					}
					sumHpPer *= 100;
					sumHpPer /= data.size();

					msg.Append(data.at(0)->getName())
					   .Append(": ").Append(Urho3D::String(data.size())).Append(" | ")
					   .Append(asStringF(sumHpPer, 1).c_str()).Append("%\n");
				}
			}
			text->SetText(msg);
			setVisible(true);
		} else {
			setVisible(false);
		}
		if (!categoryMessage.Empty()) {
			text->SetText(categoryMessage);
			setVisible(true);
		}
	}
}

Urho3D::String LeftMenuInfoPanel::createMessage(HudData* hudData) {
	const auto id = hudData->getId();
	switch (hudData->getType()) {
	case ActionType::UNIT_CREATE: {
		const auto dbUnit = Game::getDatabase()->getUnit(id);
		auto message = stringFrom(dbUnit->name, dbUnit);
		if (const auto* level = Game::getPlayersMan()->getActivePlayer()->getUnitLevel(id)) {
			appendUnitStats(message, level, false);
		}
		return message;
	}
	case ActionType::UNIT_LEVEL: {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextUnitLevel(id);
		if (opt.has_value()) {
			const auto dbLevel = opt.value();
			auto message = stringFrom(dbLevel->name, dbLevel);
			appendUnitStats(message, dbLevel, true);
			return message;
		}
		break;
	}
	case ActionType::BUILDING_CREATE: {
		const auto dbBuilding = Game::getDatabase()->getBuilding(id);
		auto message = stringFrom(dbBuilding->name, dbBuilding);
		if (const auto* level = Game::getPlayersMan()->getActivePlayer()->getBuildingLevel(id)) {
			appendBuildingStats(message, dbBuilding, level, false);
		}
		return message;
	}
	case ActionType::BUILDING_LEVEL: {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextBuildingLevel(id);

		if (opt.has_value()) {
			auto dbLevel = opt.value();
			auto message = stringFrom(dbLevel->name, dbLevel);
			appendBuildingStats(message, Game::getDatabase()->getBuilding(id), dbLevel, true);
			return message;
		}
		break;
	}
	case ActionType::TECH_RESEARCH: {
		const auto technologyLevels = Game::getDatabase()->getTechnologyLevels();
		if (id >= 0 && static_cast<size_t>(id) < technologyLevels.size()) {
			const auto* level = technologyLevels[id];
			if (level) {
				const auto cost = Game::getPlayersMan()->getActivePlayer()->technologyResearchCost(level->id);
				auto name = Game::getLocalization()->Get(level->name);
				if (const auto description = TechnologyDescription::effects(level); !description.Empty()) {
					name += "\n";
					name += description;
				}
				return stringFrom(name, &cost);
			}
		}
		break;
	}
	case ActionType::ORDER: {
		return Game::getLocalization()->Get(Urho3D::String("ord_") + magic_enum::enum_name(UnitOrderType(id)).data());
	}
	case ActionType::FORMATION:
	case ActionType::RESOURCE:
		return hudData->getText();
	default: ;
	}
	return {};
}

void LeftMenuInfoPanel::setHoverInfo(HudData* hudData) {
	Urho3D::String message = createMessage(hudData);

	if (message.Length() > 0) {
		text->SetText(message);
		setVisible(true);
		hoverIsOn = true;
	} else {
		removeHoverInfo();
	}

}

void LeftMenuInfoPanel::setCategoryMessage(const Urho3D::String& message) {
	categoryMessage = message;
	if (!hoverIsOn) {
		text->SetText(categoryMessage);
		setVisible(true);
	}
}

void LeftMenuInfoPanel::clearCategoryMessage() {
	categoryMessage.Clear();
}

void LeftMenuInfoPanel::removeHoverInfo() {
	hoverIsOn = false;
	if (!categoryMessage.Empty()) {
		text->SetText(categoryMessage);
		setVisible(true);
	} else if (!selectedIsOn) {
		setVisible(false);
	}
}

void LeftMenuInfoPanel::clearSelected() {
	selectedIsOn = false;
	if (!categoryMessage.Empty() && !hoverIsOn) {
		text->SetText(categoryMessage);
		setVisible(true);
	} else if (!hoverIsOn) {
		setVisible(false);
	}
}
