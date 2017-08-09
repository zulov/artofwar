#include "SelectedHudPanel.h"


SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style, Window* _window) {
	style = _style;
	window = _window;
	elements = new SelectedHudElement*[LINES_IN_SELECTION * MAX_ICON_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		elements[i] = new SelectedHudElement(style);
	}

	test = new UIElement*[LINES_IN_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		test[i] = window->CreateChild<UIElement>();
		test[i]->SetStyle("MyListRow", style);
	}

	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		for (int j = 0; j < MAX_ICON_SELECTION; ++j) {
			test[i]->AddChild(elements[i * MAX_ICON_SELECTION + j]->getButton());
		}
	}
}

SelectedHudPanel::~SelectedHudPanel() {
}

void SelectedHudPanel::hide() {
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		elements[i]->hide();
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
	int ratio = all / (LINES_IN_SELECTION * MAX_ICON_SELECTION - selectedSubTypeNumber + 2) + 1;
	int k = 0;
	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		std::vector<Physical*>* data = infoTypes[i]->getData();
		if (data->empty()) { continue; }
		String name = getName(type, i);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + name);
		for (int j = 0; j < data->size(); j += ratio) {
			int max = Min(data->size(), j + ratio);
			int diff = max - j;
			std::vector<Physical*>* sub = new vector<Physical*>(data->begin()+j, data->begin() + max);
			elements[k]->add(sub);
			elements[k]->show();
			elements[k]->setTexture(texture);
			elements[k]->hideText();

			if (diff > 1) {
				elements[k]->setText(String(diff));
			}
			++k;
			delete sub;
		}
	}
}
