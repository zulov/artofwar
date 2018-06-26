#include "TopPanel.h"
#include "../../UiUtils.h"
#include "GameState.h"
#include "database/DatabaseCache.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "TopHudElement.h"

TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "TopWindow";
	const int size = Game::getDatabaseCache()->getResourceSize();

	elements = new TopHudElement*[size];
	visibleAt[static_cast<char>(GameState::RUNNING)] = true;
	visibleAt[static_cast<char>(GameState::PAUSE)] = true;
}


TopPanel::~TopPanel() {
	const int size = Game::getDatabaseCache()->getResourceSize();
	for (int i = 0; i < size; ++i) {
		delete elements[i];
	}
	delete[] elements;
}

void TopPanel::createBody() {
	unitsNumber = window->CreateChild<Text>();
	unitsNumber->SetStyle("TopText", style);
	unitsNumber->SetText("Test");

	const int size = Game::getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::getDatabaseCache()->getResource(i);
		Texture2D* texture = Game::getCache()->GetResource<Texture2D>("textures/hud/icon/resource/" + resource->icon);

		elements[i] = new TopHudElement(style, texture);
		window->AddChild(elements[i]->getButton());
	}
}

void TopPanel::update(int value) const {
	unitsNumber->SetText(String(value));
}

void TopPanel::update(Resources& resources) const {
	if (resources.hasChanged()) {

		short size = resources.getSize();
		float* values = resources.getValues();
		for (int i = 0; i < size; ++i) {
			elements[i]->setText(String((int)values[i]));
		}
		resources.hasBeedUpdatedDrawn();
	}
}
