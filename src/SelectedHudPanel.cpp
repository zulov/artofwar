#include "SelectedHudPanel.h"


SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style):AbstractWindowPanel(_style) {
	styleName = "SelectedInfoWindow";
}

SelectedHudPanel::~SelectedHudPanel() {
	delete buttons;
}

void SelectedHudPanel::hide() {
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		elements[i]->hide();
	}
}

std::vector<Button*>* SelectedHudPanel::getButtonsSelectedToSubscribe() {
	return buttons;
}

void SelectedHudPanel::createBody() {
	elements = new SelectedHudElement*[LINES_IN_SELECTION * MAX_ICON_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		elements[i] = new SelectedHudElement(style);
	}

	rows = new UIElement*[LINES_IN_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		rows[i] = window->CreateChild<UIElement>();
		rows[i]->SetStyle("MyListRow", style);
	}

	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		for (int j = 0; j < MAX_ICON_SELECTION; ++j) {
			rows[i]->AddChild(elements[i * MAX_ICON_SELECTION + j]->getButton());
		}
	}

	buttons = new std::vector<Button*>();
	buttons->reserve(LINES_IN_SELECTION * MAX_ICON_SELECTION);
	for (int i = 0; i < LINES_IN_SELECTION * MAX_ICON_SELECTION; ++i) {
		buttons->push_back(elements[i]->getButton());
	}
}

String SelectedHudPanel::getIconName(ObjectType index, int i) {
	DatabaseCache* dbCache = Game::get()->getDatabaseCache();
	switch (index) {
	case ENTITY:
		return "mock.png";
	case UNIT:
		return dbCache->getUnit(i)->icon;
	case BUILDING:
		return dbCache->getBuilding(i)->icon;
	case RESOURCE:
		return dbCache->getResource(i)->icon;
	default:
		return "mock.png";
	}
}

void SelectedHudPanel::update(SelectedInfo* selectedInfo) {
	hide();
	ObjectType type = selectedInfo->getSelectedType();
	vector<SelectedInfoType*>* infoTypes = selectedInfo->getSelecteType();

	int all = selectedInfo->getAllNumber();
	int selectedSubTypeNumber = selectedInfo->getSelectedSubTypeNumber();
	int ratio = all / (LINES_IN_SELECTION * MAX_ICON_SELECTION - selectedSubTypeNumber + 2) + 1;
	int k = 0;
	for (int i = 0; i < infoTypes->size(); ++i) {
		std::vector<Physical*>* data = infoTypes->at(i)->getData();
		if (data->empty()) { continue; }
		String name = getIconName(type, infoTypes->at(i)->getId());
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + name);
		for (int j = 0; j < data->size(); j += ratio) {
			int max = Min(data->size(), j + ratio);
			int diff = max - j;
			std::vector<Physical*>* sub = new vector<Physical*>(data->begin() + j, data->begin() + max);
			elements[k]->add(sub);
			elements[k]->show();
			elements[k]->setTexture(texture);
			
			if (diff > 1) {
				elements[k]->setText(String(diff));
			}else {
				elements[k]->hideText();
			}
			++k;
			delete sub;
		}
	}
}
