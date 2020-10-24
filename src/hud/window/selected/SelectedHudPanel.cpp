#include "SelectedHudPanel.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Window.h>
#include "GameState.h"
#include "SelectedHudElement.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include "objects/NamesCache.h"


SelectedHudPanel::SelectedHudPanel(Urho3D::XMLFile* _style)
	: SimplePanel(_style, "SelectedInfoWindow",
	                      {GameState::RUNNING, GameState::PAUSE}) {
}

SelectedHudPanel::~SelectedHudPanel() {
	clear_array(elements, linesNumber * maxInRow);

	delete[]elements;
	delete[]rows;
}

void SelectedHudPanel::hide(int i) {
	const auto lh = lastHidden;
	lastHidden = i;
	for (; i < lh; ++i) {
		elements[i]->hide();
	}
}

std::vector<Urho3D::Button*> SelectedHudPanel::getButtonsSelectedToSubscribe() const {
	std::vector<Urho3D::Button*> buttons;
	buttons.reserve(linesNumber * maxInRow);
	for (int i = 0; i < linesNumber * maxInRow; ++i) {
		buttons.push_back(elements[i]->getButton());
	}
	return buttons;
}

void SelectedHudPanel::createBody() {
	createRows();

	int border = rows[0]->GetLayoutBorder().left_ + rows[0]->GetLayoutBorder().right_
		+ window->GetLayoutBorder().left_ + window->GetLayoutBorder().right_;
	int space = window->GetSize().x_ - border;

	maxInRow = space / iconSize();

	elements = new SelectedHudElement*[linesNumber * maxInRow];

	for (int i = 0; i < linesNumber; ++i) {
		for (int j = 0; j < maxInRow; ++j) {
			elements[i * maxInRow + j] = new SelectedHudElement(rows[i], style);
		}
	}

	lastHidden = linesNumber * maxInRow;
}

int SelectedHudPanel::iconSize() const {
	auto test = new Urho3D::UIElement(Game::getContext());
	test->SetStyle("Icon", style);

	int size = test->GetSize().x_ + rows[0]->GetLayoutSpacing();
	test->Remove();
	delete test;
	return size;
}

void SelectedHudPanel::createRows() {
	rows = new Urho3D::UIElement*[linesNumber];
	for (int i = 0; i < linesNumber; ++i) {
		rows[i] = createElement<Urho3D::UIElement>(window, style, "MyListRow");
	}
}

void SelectedHudPanel::clearSelected() {
	hide(0);
}

void SelectedHudPanel::update(SelectedInfo* selectedInfo) {
	ObjectType type = selectedInfo->getSelectedType();

	int all = selectedInfo->getAllNumber();
	auto selectedSubTypeNumber = selectedInfo->getSelectedSubTypeNumber();
	int ratio = all / (linesNumber * maxInRow - selectedSubTypeNumber + 2) + 1;
	int k = 0;
	for (auto& infoType : selectedInfo->getSelectedTypes()) {
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
			if (k >= linesNumber * maxInRow) {
				break;
			}
		}
	}
	hide(k);
}
