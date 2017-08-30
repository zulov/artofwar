#include "OrdersPanel.h"
#include "Game.h"
#include <Urho3D/UI/ListView.h>
#include "MySprite.h"
#include <Urho3D/UI/Button.h>
#include "ButtonUtils.h"


OrdersPanel::OrdersPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "OrdersWindow";
}


OrdersPanel::~OrdersPanel() {
}

void OrdersPanel::show(SelectedInfo* selectedInfo) {
	setVisible(true);
}

std::vector<HudElement*>* OrdersPanel::getButtons() {
	return  buttons;
}

void OrdersPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getOrdersSize();
	buttons = new std::vector<HudElement*>();
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
