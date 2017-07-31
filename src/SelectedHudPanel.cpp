#include "SelectedHudPanel.h"


SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style, Window* _window) {
	style = _style;
	window = _window;
	buttons = new Button*[LINES_IN_SELECTION * MAX_ICON_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		buttons[i] = simpleButton(nullptr, style, "SmallIcon");
	}

	test = new UIElement*[LINES_IN_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		test[i] = window->CreateChild<UIElement>();
		test[i]->SetStyle("MyListRow", style);
	}

	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		for (int j = 0; j < MAX_ICON_SELECTION; ++j) {
			test[i]->AddChild(buttons[i * LINES_IN_SELECTION + j]);
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
	int size = getSize(type);
	//	String** lines = selectedInfo->getLines();
	//	for (int i = 0; i < size; ++i) {
	//		if ((lines[i]) != nullptr) {			
	//			buttons[selectedInfo->getSelectedType()][i]->SetVisible(true);
	//		}
	//	}

	double** hps = selectedInfo->getHps();
	int k = 0;
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		for (int j = 0; j < SELECTED_INFO_SIZE_2; ++j) {
			if (hps[i][j] != -1) {
				buttons[k++]->SetVisible(true);
			}
		}
	}
}
