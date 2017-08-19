#include "BuildPanel.h"
#include "BuildingType.h"
#include "HudElement.h"
#include "ButtonUtils.h"
#include <Urho3D/UI/ListView.h>


BuildPanel::BuildPanel(Urho3D::XMLFile* _style, int _nation): AbstractWindowPanel(_style) {
	nation = _nation;
	styleName = "BuildWindow";
}


BuildPanel::~BuildPanel() {
	delete buttons;
}

std::vector<HudElement*>* BuildPanel::getButtons() {
	return buttons;
}

void BuildPanel::show(SelectedInfo* selectedInfo) {
	setVisible(true);
}

void BuildPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getBuildingSize();
	buttons = new std::vector<HudElement*>();
	buttons->reserve(size);
	ListView* panel = window->CreateChild<ListView>();
	panel->SetStyle("MyListView", style);

	for (int i = 0; i < size; ++i) {
		db_building* building = Game::get()->getDatabaseCache()->getBuilding(i);
		if (building == nullptr || building->nation != nation) { continue; }
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + building->icon);

		MySprite* sprite = createSprite(texture, style, "Sprite");
		Button* button = simpleButton(sprite, style, "Icon");

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ObjectType::BUILDING);

		button->SetVar("HudElement", hudElement);
		buttons->push_back(hudElement);
		panel->AddItem(button);
	}
	BorderImage* element = panel->CreateChild<BorderImage>();
	element->SetStyle("EditorDivider", style);
	panel->AddItem(element);
	
}
