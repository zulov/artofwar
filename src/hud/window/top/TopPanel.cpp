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
	elements = new TopHudElement*[Game::getDatabaseCache()->getResourceSize() + 1];
}


TopPanel::~TopPanel() {
	const int size = Game::getDatabaseCache()->getResourceSize() + 1;
	for (int i = 0; i < size; ++i) {
		delete elements[i];
	}
	delete[] elements;
	delete infoPanel;
}

void TopPanel::createBody() {
	const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/top/human.png");
	elements[0] = new TopHudElement(window, style, texture);
	const int size = Game::getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		const auto resource = Game::getDatabaseCache()->getResource(i);
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>(
			"textures/hud/icon/resource/" + resource->icon);

		elements[i + 1] = new TopHudElement(window, style, texture);
	}
	infoPanel = new TopInfoPanel(style);
	infoPanel->createWindow();
	infoPanel->setVisible(true);
}

void TopPanel::update(int value) const {
	elements[0]->setText(Urho3D::String(value));
}

void TopPanel::update(Resources& resources) const {
	if (resources.hasChanged()) {
		short size = resources.getSize();
		float* values = resources.getValues();
		for (int i = 0; i < size; ++i) {
			elements[i + 1]->setText(Urho3D::String((int)values[i]));
		}
		resources.hasBeedUpdatedDrawn();
	}
}

void TopPanel::setVisible(bool enable) {
	AbstractWindowPanel::setVisible(enable);
	//if (!enable) {
		infoPanel->setVisible(enable);
	//}
}
