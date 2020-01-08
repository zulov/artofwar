#include "SelectedHudPanel.h"
#include "GameState.h"
#include "SelectedHudElement.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include "objects/NamesCache.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>


SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style, "SelectedInfoWindow",
                                                                                 {
	                                                                                 GameState::RUNNING,
	                                                                                 GameState::PAUSE
                                                                                 }) {
}

SelectedHudPanel::~SelectedHudPanel() {
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

std::vector<Urho3D::Button*>& SelectedHudPanel::getButtonsSelectedToSubscribe() {
	return buttons;
}

void SelectedHudPanel::createBody() {
	createRows();

	int border = rows[0]->GetLayoutBorder().left_ + rows[0]->GetLayoutBorder().right_
		+ window->GetLayoutBorder().left_ + window->GetLayoutBorder().right_;
	int space = window->GetSize().x_ - border;

	maxInRow = space / iconSize();

	elements = new SelectedHudElement*[LINES_IN_SELECTION * maxInRow];

	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		for (int j = 0; j < maxInRow; ++j) {
			elements[i * maxInRow + j] = new SelectedHudElement(rows[i], style);
		}
	}

	buttons.reserve(LINES_IN_SELECTION * maxInRow);
	for (int i = 0; i < LINES_IN_SELECTION * maxInRow; ++i) {
		buttons.push_back(elements[i]->getButton());
	}
}

int SelectedHudPanel::iconSize() {
	auto test = new Urho3D::UIElement(Game::getContext());
	test->SetStyle("Icon", style);

	int size = test->GetSize().x_ + rows[0]->GetLayoutSpacing();
	test->Remove();
	delete test;
	return size;
}

void SelectedHudPanel::createRows() {
	rows = new Urho3D::UIElement*[LINES_IN_SELECTION];
	for (int i = 0; i < LINES_IN_SELECTION; ++i) {
		rows[i] = createElement<Urho3D::UIElement>(window, style, "MyListRow");
	}
}

void SelectedHudPanel::clearSelected() {
	hide(0);
}

void SelectedHudPanel::update(SelectedInfo* selectedInfo) {
	ObjectType type = selectedInfo->getSelectedType();
	auto& infoTypes = selectedInfo->getSelectedTypes();

	int all = selectedInfo->getAllNumber();
	auto selectedSubTypeNumber = selectedInfo->getSelectedSubTypeNumber();
	int ratio = all / (LINES_IN_SELECTION * maxInRow - selectedSubTypeNumber + 2) + 1;
	int k = 0;
	for (auto& infoType : infoTypes) {
		auto& data = infoType->getData();
		if (data.empty()) { continue; }
		auto name = getIconName(type, infoType->getId());
		auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/" + name);

		for (int j = 0; j < data.size(); j += ratio) {
			int max = Urho3D::Min(data.size(), j + ratio);
			int diff = max - j;

			elements[k]->add(data, j, max);
			elements[k]->show();
			elements[k]->setTexture(texture);

			if (diff > 1) {
				elements[k]->setText(Urho3D::String(diff));
			} else {
				elements[k]->hideText();
			}
			++k;
			if (k >= LINES_IN_SELECTION * maxInRow) {
				break;
			}
		}
	}
	hide(k);
}
