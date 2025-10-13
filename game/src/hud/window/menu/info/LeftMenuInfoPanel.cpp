#include "LeftMenuInfoPanel.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Window.h>
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/db_other_struct.h"
#include "hud/HudData.h"
#include "hud/UiUtils.h"
#include "objects/ActionType.h"
#include "objects/NamesCache.h"
#include "objects/Physical.h"
#include "objects/unit/order/UnitConst.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "utils/NamesUtils.h"

LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : SimplePanel(root, _style, "LeftMenuInfoPanel", {}) {
}

void LeftMenuInfoPanel::createBody() {
	text = addChildText(window, "MyText", style);
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
	}
}

Urho3D::String LeftMenuInfoPanel::createMessage(HudData* hudData) {
	const auto id = hudData->getId();
	switch (hudData->getType()) {
	case ActionType::UNIT_CREATE: {
		const auto dbUnit = Game::getDatabase()->getUnit(id);

		return stringFrom(dbUnit->name, dbUnit);
	}
	case ActionType::UNIT_LEVEL: {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForUnit(id);
		if (opt.has_value()) {
			const auto dbLevel = opt.value();
			return stringFrom(dbLevel->name, dbLevel);
		}
	}
	case ActionType::UNIT_UPGRADE:
		return "TODO";
	case ActionType::BUILDING_CREATE: {
		const auto dbBuilding = Game::getDatabase()->getBuilding(id);

		return stringFrom(dbBuilding->name, dbBuilding);
	}
	case ActionType::BUILDING_LEVEL: {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForBuilding(id);

		if (opt.has_value()) {
			auto dbLevel = opt.value();
			return stringFrom(dbLevel->name, dbLevel);
		}
	}
	case ActionType::ORDER: {
		std::string str = "ord_";
		str.append(magic_enum::enum_name(UnitOrderType(id)).data());
		return Game::getLocalization()->Get(str.c_str());
	}
	case ActionType::FORMATION:
	case ActionType::RESOURCE:
		return hudData->getText();
	default: ;
	}
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

void LeftMenuInfoPanel::removeHoverInfo() {
	hoverIsOn = false;
	if (!selectedIsOn) {
		setVisible(false);
	}
}

void LeftMenuInfoPanel::clearSelected() {
	selectedIsOn = false;
	if (!hoverIsOn) {
		setVisible(false);
	}
}
