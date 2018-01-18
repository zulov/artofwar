#include "UnitsPanel.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include "hud/HudElement.h"
#include "hud/MySprite.h"
#include "utils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/ListView.h>
#include <algorithm>
#include <unordered_set>
#include "GameState.h"
#include "player/PlayersManager.h"


UnitsPanel::UnitsPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "UnitsWindow";

	visibleAt.insert(GameState::RUNNING);
	visibleAt.insert(GameState::PAUSE);
}


UnitsPanel::~UnitsPanel() {
	clear_vector(buttons);
}

std::vector<HudElement*>& UnitsPanel::getButtons() {
	return buttons;
}

void UnitsPanel::show(SelectedInfo* selectedInfo) {
	setVisible(true);
	vector<SelectedInfoType*> infoTypes = selectedInfo->getSelecteType();

	unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int nation = Game::get()->getPlayersManager()->getActivePlayer()->getNation();
	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			std::vector<db_unit*>* units = Game::get()->getDatabaseCache()->getUnitsForBuilding(i);
			unordered_set<int> common2;
			for (auto& unit : *units) {
				//todo to zrobic raz i pobierac
				if (unit->nation == nation) {
					common2.insert(unit->id);
				}
			}
			unordered_set<int> temp(common);
			for (const auto& id : temp) {
				if (common2.find(id) == common2.end()) {
					common.erase(id);
				}
			}
		}
	}

	for (int i = 0; i < buttons.size(); ++i) {
		if (common.find(i) != common.end()) {
			buttons.at(i)->getUIElement()->SetVisible(true);
		} else {
			buttons.at(i)->getUIElement()->SetVisible(false);
		}
	}
}

void UnitsPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getUnitSize();

	buttons.reserve(size);
	ListView* panel = window->CreateChild<ListView>();
	panel->SetStyle("MyListView", style);

	for (int i = 0; i < size; ++i) {
		db_unit* unit = Game::get()->getDatabaseCache()->getUnit(i);
		if (unit) {
			Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + unit->icon);

			MySprite* sprite = createSprite(texture, style, "Sprite");
			Button* button = simpleButton(sprite, style, "Icon");

			HudElement* hudElement = new HudElement(button);
			hudElement->setId(i, ObjectType::UNIT);

			button->SetVar("HudElement", hudElement);
			buttons.push_back(hudElement);
			panel->AddItem(button);
		}
	}
}
