#include "SelectedHudPanel.h"
#include <Urho3D/Resource/ResourceCache.h>
#include "database/DatabaseCache.h"
#include <Urho3D/Graphics/Texture2D.h>

SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "SelectedInfoWindow";

	buttons = new std::vector<Button*>();
}

SelectedHudPanel::~SelectedHudPanel() {
	delete buttons;
	for (int i = 0; i < LINES_IN_SELECTION * maxInRow; ++i) {
		delete elements[i];
	}
	delete[]elements;
	delete[]rows;
}

void SelectedHudPanel::hide(int i) {
	for (; i < LINES_IN_SELECTION * maxInRow; ++i) {
		elements[i]->hide();
	}
}

std::vector<Button*>* SelectedHudPanel::getButtonsSelectedToSubscribe() {
	return buttons;
}

void SelectedHudPanel::createBody() {
	createRows();

	int border = rows[0]->GetLayoutBorder().left_ + rows[0]->GetLayoutBorder().right_ + window->GetLayoutBorder().left_ +
		window->
		GetLayoutBorder().right_;
	int space = window->GetSize().x_ - border;

	int size = iconSize();

	maxInRow = space / size;

	elements = new SelectedHudElement*[LINES_IN_SELECTION * maxInRow];

	for (int i = 0; i < LINES_IN_SELECTION * maxInRow; ++i) {
		elements[i] = new SelectedHudElement(style);
	}

	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		for (int j = 0; j < maxInRow; ++j) {
			rows[i]->AddChild(elements[i * maxInRow + j]->getButton());
		}
	}

	buttons->reserve(LINES_IN_SELECTION * maxInRow);
	for (int i = 0; i < LINES_IN_SELECTION * maxInRow; ++i) {
		buttons->push_back(elements[i]->getButton());
	}
}

int SelectedHudPanel::iconSize() {
	UIElement* test = new UIElement(Game::get()->getContext());
	test->SetStyle("SmallIcon", style);

	int size = test->GetSize().x_ + rows[0]->GetLayoutSpacing();
	test->Remove();
	delete test;
	return size;
}

void SelectedHudPanel::createRows() {
	rows = new UIElement*[LINES_IN_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		rows[i] = window->CreateChild<UIElement>();
		rows[i]->SetStyle("MyListRow", style);
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
	ObjectType type = selectedInfo->getSelectedType();
	std::vector<SelectedInfoType*>* infoTypes = selectedInfo->getSelecteType();

	int all = selectedInfo->getAllNumber();
	int selectedSubTypeNumber = selectedInfo->getSelectedSubTypeNumber();
	int ratio = all / (LINES_IN_SELECTION * maxInRow - selectedSubTypeNumber + 2) + 1;
	int k = 0;
	for (auto & infoType : *infoTypes) {
		std::vector<Physical*>* data = infoType->getData();
		if (data->empty()) { continue; }
		String name = getIconName(type, infoType->getId());
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + name);
		for (int j = 0; j < data->size(); j += ratio) {
			int max = Min(data->size(), j + ratio);
			int diff = max - j;

			elements[k]->add(data, j, max);
			elements[k]->show();
			elements[k]->setTexture(texture);

			if (diff > 1) {
				elements[k]->setText(String(diff));
			} else {
				elements[k]->hideText();
			}
			++k;
		}
	}
	hide(k);
}
