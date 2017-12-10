#include "TopPanel.h"
#include "Game.h"
#include "../../ButtonUtils.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>

TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "TopWindow";
}


TopPanel::~TopPanel() {
	delete buttons;
	int size = Game::get()->getDatabaseCache()->getResourceSize();
	for (int i = 0; i < size; ++i) {
		delete elements[i];
	}
	delete[] elements;
}

void TopPanel::createBody() {
	unitsNumber = window->CreateChild<Text>();
	unitsNumber->SetStyle("TopText", style);
	unitsNumber->SetText("Test");

	int size = Game::get()->getDatabaseCache()->getResourceSize();

	elements = new TopHudElement*[size];
	buttons = new std::vector<Button*>();
	buttons->reserve(size);
	for (int i = 0; i < size; ++i) {
		db_resource* resource = Game::get()->getDatabaseCache()->getResource(i);
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D>("textures/hud/icon/" + resource->icon);

		elements[i] = new TopHudElement(style, texture);
		window->AddChild(elements[i]->getButton());
		buttons->push_back(elements[i]->getButton());
	}
}

std::vector<Button*>* TopPanel::getButtonsSelectedToSubscribe() {
	return buttons;
}

void TopPanel::update(int value) {
	unitsNumber->SetText(String(value));
}

void TopPanel::update(Resources* resources) {
	if (resources->hasChanged()) {

		short size = resources->getSize();
		double* values = resources->getValues();
		for (int i = 0; i < size; ++i) {
			elements[i]->setText(String((int)values[i]));
		}
		resources->hasBeedUpdatedDrawn();
	}
}
