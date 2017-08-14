#include "UnitsPanel.h"
#include "Game.h"
#include <Urho3D/UI/ListView.h>
#include "MySprite.h"
#include "ButtonUtils.h"
#include "HudElement.h"


UnitsPanel::UnitsPanel(Urho3D::XMLFile* _style, int _nation): AbstractWindowPanel(_style) {
	nation = _nation;
	styleName = "UnitsWindow";
}


UnitsPanel::~UnitsPanel() {
	delete buttons;
}

std::vector<HudElement*>* UnitsPanel::getButtons() {
	return buttons;
}

void UnitsPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getUnitSize();
	buttons = new std::vector<HudElement*>();
	buttons->reserve(size);
	ListView* panel = window->CreateChild<ListView>();
	panel->SetStyle("MyListView", style);

	for (int i = 0; i < size; ++i) {
		db_unit* unit = Game::get()->getDatabaseCache()->getUnit(i);
		if (unit == nullptr || unit->nation != nation) { continue; }
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + unit->icon);

		MySprite* sprite = createSprite(texture, style, "Sprite");
		Button* button = simpleButton(sprite, style, "Icon");

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ObjectType::UNIT);

		button->SetVar("HudElement", hudElement);
		buttons->push_back(hudElement);
		panel->AddItem(button);
	}
}
