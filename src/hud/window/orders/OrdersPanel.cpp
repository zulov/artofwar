#include "OrdersPanel.h"
#include "../../ButtonUtils.h"
#include "../../MySprite.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "utils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ListView.h>
#include <unordered_set>



OrdersPanel::OrdersPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "OrdersWindow";
	buttons = new std::vector<HudElement*>();
}


OrdersPanel::~OrdersPanel() {
	clear_and_delete_vector(buttons);
}

void OrdersPanel::show(SelectedInfo* selectedInfo) {
	setVisible(true);
	std::vector<SelectedInfoType*>& infoTypes = selectedInfo->getSelecteType();

	std::unordered_set<int> common = {0,1,2,3,4,5,6,7,8,9,10};

	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			std::vector<db_order*>* orders = Game::get()->getDatabaseCache()->getOrdersForUnit(i);
			std::unordered_set<int> common2;
			for (auto & order : *orders) {
				//todo to zrobic raz i pobierac
				common2.insert(order->id);
			}
			std::unordered_set<int> temp(common);
			for (const auto& id : temp) {
				if (common2.find(id) == common2.end()) {
					common.erase(id);
				}
			}
		}
	}

	for (int i = 0; i < buttons->size(); ++i) {
		if (common.find(i) != common.end()) {
			buttons->at(i)->getUIElement()->SetVisible(true);
		} else {
			buttons->at(i)->getUIElement()->SetVisible(false);
		}
	}
}

std::vector<HudElement*>* OrdersPanel::getButtons() {
	return buttons;
}

void OrdersPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getOrdersSize();

	buttons->reserve(size);
	ListView* panel = window->CreateChild<ListView>();
	panel->SetStyle("MyListView", style);

	for (int i = 0; i < size; ++i) {
		db_order* order = Game::get()->getDatabaseCache()->getOrder(i);
		if (order == nullptr) { continue; }
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/orders/" + order->icon);

		MySprite* sprite = createSprite(texture, style, "Sprite");
		Button* button = simpleButton(sprite, style, "Icon");

		HudElement* hudElement = new HudElement(button);//TODO inny obiekt
		hudElement->setId(i, ObjectType::ENTITY);

		button->SetVar("HudElement", hudElement);
		buttons->push_back(hudElement);
		panel->AddItem(button);
	}
}
