#include "TopPanel.h"
#include "../../UiUtils.h"
#include "GameState.h"
#include "TopHudElement.h"
#include "database/DatabaseCache.h"
#include "player/Resources.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "player/Possession.h"


TopPanel::TopPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "TopWindow",
                                                                  {GameState::RUNNING, GameState::PAUSE}) {
	size = Game::getDatabaseCache()->getResourceSize() + 2;
	elements = new TopHudElement*[size];
}


TopPanel::~TopPanel() {
	for (int i = 0; i < size; ++i) {
		delete elements[i];
	}
	delete[] elements;
	delete infoPanel;
}

void TopPanel::createBody() {
	const auto textureHuman = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/top/human.png");
	const auto textureWorker = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/top/worker.png");
	elements[0] = new TopHudElement(window, style, textureHuman);
	elements[1] = new TopHudElement(window, style, textureWorker);
	const int size = Game::getDatabaseCache()->getResourceSize();

	for (int i = 0; i < size; ++i) {
		const auto resource = Game::getDatabaseCache()->getResource(i);
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>(
			"textures/hud/icon/resource/" + resource->icon);

		elements[i + 2] = new TopHudElement(window, style, texture);
	}
	infoPanel = new TopInfoPanel(style);
	infoPanel->createWindow();
	infoPanel->setVisible(true);
}

void TopPanel::update(Possession& possession) const {
	elements[0]->setText(Urho3D::String(possession.getUnitsNumber()));
	elements[1]->setText(Urho3D::String(possession.getWorkersNumber()));
}

void TopPanel::update(Resources& resources) const {
	if (resources.hasChanged()) {
		short size = resources.getSize();
		float* values = resources.getValues();
		for (int i = 0; i < size; ++i) {
			elements[i + 2]->setText(Urho3D::String((int)values[i]));
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
