#include "TopPanel.h"
#include "../../UiUtils.h"
#include "GameState.h"
#include "TopHudElement.h"
#include "database/DatabaseCache.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>


TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "TopWindow",
                                                                  {GameState::RUNNING, GameState::PAUSE}) {
	elements = new TopHudElement*[Game::getDatabaseCache()->getResourceSize()];
}


TopPanel::~TopPanel() {
	const int size = Game::getDatabaseCache()->getResourceSize();
	for (int i = 0; i < size; ++i) {
		delete elements[i];
	}
	delete[] elements;
}

void TopPanel::createBody() {
	unitsNumber = addChildText(window, "TopText", "Test", style);

	const int size = Game::getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		const auto resource = Game::getDatabaseCache()->getResource(i);
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/resource/" + resource->icon);

		elements[i] = new TopHudElement(window, style, texture);
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
