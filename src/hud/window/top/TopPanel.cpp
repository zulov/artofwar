#include "TopPanel.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Window.h>
#include "GameState.h"
#include "TopHudElement.h"
#include "../../UiUtils.h"
#include "database/DatabaseCache.h"
#include "info/TopInfoPanel.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"


TopPanel::TopPanel(Urho3D::XMLFile* _style) : SimplePanel(_style, "TopWindow",
                                                                  {GameState::RUNNING, GameState::PAUSE}) {
	elements = new TopHudElement*[Game::getDatabase()->getResourceSize()];
}


TopPanel::~TopPanel() {
	clear_array(elements, Game::getDatabase()->getResourceSize());
	delete units;
	delete workers;
	delete name;
	delete[] elements;
	delete infoPanel;
}

void TopPanel::createBody() {
	const auto textureHuman = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/top/human.png");
	const auto textureWorker = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/top/worker.png");
	const auto textureName = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/top/helmet.png");
	name = new TopHudElement(window, style, textureName);
	units = new TopHudElement(window, style, textureHuman, "TopButtonsNarrow");
	workers = new TopHudElement(window, style, textureWorker, "TopButtonsNarrow");

	for (int i = 0; i < Game::getDatabase()->getResourceSize(); ++i) {
		const auto resource = Game::getDatabase()->getResource(i);
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>(
			"textures/hud/icon/resource/" + resource->icon);

		elements[i] = new TopHudElement(window, style, texture);
	}
	infoPanel = new TopInfoPanel(style);
	infoPanel->createWindow();
	infoPanel->setVisible(true);
}

void TopPanel::update(Player* player) const {
	auto& possession = player->getPossession();

	units->setText(Urho3D::String(possession.getUnitsNumber()));
	workers->setText(Urho3D::String(possession.getWorkersNumber()));
	name->setText(Urho3D::String(player->getName()));

	auto& resources = player->getResources();
	if (resources.hasChanged()) {
		auto vals = resources.getValues();

		for (int i = 0; i < vals.size(); ++i) {
			elements[i]->setText(Urho3D::String((int)vals[i]));
		}
		resources.hasBeenUpdatedDrawn();
	}
}

void TopPanel::setVisible(bool enable) {
	SimplePanel::setVisible(enable);
	//if (!enable) {
	infoPanel->setVisible(enable);
	//}
}
