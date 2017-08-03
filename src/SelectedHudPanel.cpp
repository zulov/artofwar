#include "SelectedHudPanel.h"


SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style, Window* _window) {
	style = _style;
	window = _window;
	buttons = new Button*[LINES_IN_SELECTION * MAX_ICON_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		buttons[i] = simpleButton(nullptr, style, "SmallIcon");
		Text* text = buttons[i]->CreateChild<Text>();
		text->SetStyle("MyText", style);
	}

	test = new UIElement*[LINES_IN_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		test[i] = window->CreateChild<UIElement>();
		test[i]->SetStyle("MyListRow", style);
	}

	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		for (int j = 0; j < MAX_ICON_SELECTION; ++j) {
			test[i]->AddChild(buttons[i * MAX_ICON_SELECTION + j]);
		}
	}
}

SelectedHudPanel::~SelectedHudPanel() {
}

void SelectedHudPanel::hide() {
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		buttons[i]->SetVisible(false);
	}
}

String SelectedHudPanel::getName(ObjectType index, int i) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (index) {
	case ENTITY:
		return "mock.png";
	case UNIT:
		return dbCache->getUnitType(i)->icon;
	case BUILDING:
		return dbCache->getBuildingType(i)->icon;
	case RESOURCE:
		return dbCache->getResource(i)->icon;
	default:
		return "mock.png";;
	}
}

int SelectedHudPanel::getSize(ObjectType type) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (type) {
	case ENTITY:
		return 0;
	case UNIT:
		return dbCache->getUnitTypeSize();
	case BUILDING:
		return dbCache->getBuildingTypeSize();
	case RESOURCE:
		return dbCache->getResourceSize();
	default:
		return 0;
	}
}

void SelectedHudPanel::updateSelected(SelectedInfo* selectedInfo) {
	hide();
	ObjectType type = selectedInfo->getSelectedType();
	SelectedInfoType** infoTypes = selectedInfo->getSelecteType();
	int all = selectedInfo->getAllNumber();
	int selectedSubTypeNumber = selectedInfo->getSelectedSubTypeNumber();
	int ratio = all / (LINES_IN_SELECTION * MAX_ICON_SELECTION - selectedSubTypeNumber+2) + 1;
	int k = 0;
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		std::vector<Physical*>* data = infoTypes[i]->getData();
		if (data->empty()) { continue; }
		String name = getName(type, i);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + name);
		int upTo = 0;
		for (int j = 0; j < data->size(); ++j) {
			Sprite* sprite = createSprite(texture, style, "SmallSprite");
			if (upTo == 0) {
				buttons[k]->SetVisible(true);
				buttons[k]->RemoveChildAtIndex(1);
				buttons[k]->AddChild(sprite);
				Text* text = (Text*)buttons[k]->GetChild(0);
				text->SetText("");
				++k;
			} else {
				Text* text = (Text*)buttons[k-1]->GetChild(0);
				text->SetText(String(upTo + 1));//TODO mozna zoptymalizowac ustawaic raz na koniec
			}
			++upTo;
			if (upTo >= ratio) {
				upTo = 0;
			}

		}
	}
}
