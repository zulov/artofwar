#include "TopPanel.h"
#include "../../UiUtils.h"
#include "GameState.h"
#include "TopHudElement.h"
#include "database/DatabaseCache.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>


TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "TopWindow") {
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
	unitsNumber = window->CreateChild<Urho3D::Text>();
	unitsNumber->SetStyle("TopText", style);
	unitsNumber->SetText("Test");

	const int size = Game::getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::getDatabaseCache()->getResource(i);
		auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/resource/" + resource->icon);

		elements[i] = new TopHudElement(style, texture);
		window->AddChild(elements[i]->getButton());
	}
}

void TopPanel::update(int value) const {
	unitsNumber->SetText(Urho3D::String(value));
}

void TopPanel::update(Resources& resources) const {
	if (resources.hasChanged()) {

		short size = resources.getSize();
		float* values = resources.getValues();
		for (int i = 0; i < size; ++i) {
			elements[i]->setText(Urho3D::String((int)values[i]));
		}
		resources.hasBeedUpdatedDrawn();
	}
}
