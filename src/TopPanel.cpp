#include "TopPanel.h"
#include "Game.h"

TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "TopWindow";
}


TopPanel::~TopPanel() {
}

void TopPanel::createBody() {
	int size = Game::get()->getDatabaseCache()->getResourceSize();

	elements = new TopHudElement*[size];
	buttons = new std::vector<Button*>();
	buttons->reserve(size);
	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + resource->icon);

		elements[i] = new TopHudElement(style, texture, resource->name);
		window->AddChild(elements[i]->getButton());
		buttons->push_back(elements[i]->getButton());
	}
}

std::vector<Button*>* TopPanel::getButtonsSelectedToSubscribe() {
	return buttons;
}

void TopPanel::update(Resources* resources) {
	if (resources->hasChanged()) {

		short size = resources->getSize();
		float* values = resources->getValues();
		for (int i = 0; i < size; ++i) {
			elements[i]->setText(String((int)values[i]));
		}
		resources->hasBeedUpdatedDrawn();
	}
}
