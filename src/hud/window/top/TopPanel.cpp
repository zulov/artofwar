#include "TopPanel.h"
#include "../../UiUtils.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "GameState.h"


TopPanel::TopPanel() : AbstractWindowPanel() {
	styleName = "TopWindow";
	const int size = Game::get()->getDatabaseCache()->getResourceSize();

	elements = new TopHudElement*[size];
	visibleAt.insert(GameState::RUNNING);
	visibleAt.insert(GameState::PAUSE);
}


TopPanel::~TopPanel() {
	const int size = Game::get()->getDatabaseCache()->getResourceSize();
	for (int i = 0; i < size; ++i) {
		delete elements[i];
	}
	delete[] elements;
}

void TopPanel::createBody() {
	unitsNumber = window->CreateChild<Text>();
	unitsNumber->SetStyle("TopText");
	unitsNumber->SetText("Test");

	const int size = Game::get()->getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + resource->icon);

		elements[i] = new TopHudElement(texture);
		window->AddChild(elements[i]->getButton());
	}
}

void TopPanel::update(int value) {
	unitsNumber->SetText(String(value));
}

void TopPanel::update(Resources& resources) {
	if (resources.hasChanged()) {

		short size = resources.getSize();
		double* values = resources.getValues();
		for (int i = 0; i < size; ++i) {
			elements[i]->setText(String((int)values[i]));
		}
		resources.hasBeedUpdatedDrawn();
	}
}
