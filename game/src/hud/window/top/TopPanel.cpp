#include "TopPanel.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Window.h>
#include "GameState.h"
#include "TopHudElement.h"
#include "../../UiUtils.h"
#include "database/DatabaseCache.h"
#include "info/TopInfoPanel.h"
#include "objects/unit/Unit.h"
#include "player/Player.h"
#include "player/Possession.h"
#include "player/Resources.h"


TopPanel::TopPanel(Urho3D::UIElement* root, Urho3D::XMLFile* style) : SimplePanel(root, style, "TopWindow",
		 {GameState::RUNNING, GameState::PAUSE}) {}


TopPanel::~TopPanel() {
	clear_array(elements, RESOURCES_SIZE);
	delete units;
	delete workers;
	delete name;
	delete infoPanel;
}

void TopPanel::createBody() {
	const Urho3D::String path = "textures/hud/icon/top/";
	const auto textureHuman = getTexture(path + "human.png");
	const auto textureWorker = getTexture(path + "worker.png");
	const auto textureName = getTexture(path + "helmet.png");
	name = new TopHudElement(window, style, textureName);
	units = new TopHudElement(window, style, textureHuman, "TopButtonsNarrow");
	workers = new TopHudElement(window, style, textureWorker, "TopButtonsNarrow");

	const Urho3D::String resIconPath = "textures/hud/icon/resource/";
	elements[0] = new TopHudElement(window, style, getTexture(resIconPath + "food.png"));
	elements[1] = new TopHudElement(window, style, getTexture(resIconPath + "wood.png"));
	elements[2] = new TopHudElement(window, style, getTexture(resIconPath + "stone.png"));
	elements[3] = new TopHudElement(window, style, getTexture(resIconPath + "gold.png"));

	infoPanel = new TopInfoPanel(root, style);
	infoPanel->createWindow();
	infoPanel->setVisible(true);
}

void TopPanel::update(Player* player) const {
	auto& poss = player->getPossession();

	name->setText(Urho3D::String(player->getName()), "");
	units->setText(Urho3D::String(poss.getFreeArmyNumber()) + "/", Urho3D::String(poss.getArmyNumber()));
	workers->setText(Urho3D::String(poss.getFreeWorkersNumber()) + "/", Urho3D::String(poss.getWorkersNumber()));

	const auto& resources = player->getResources();
	unsigned short workersPerRes[RESOURCES_SIZE] = {0, 0, 0, 0};
	for (const auto worker : poss.getWorkers()) {
		if (worker->getState() == UnitState::COLLECT) {
			const auto resId = dynamic_cast<ResourceEntity*>(worker->getThingToInteract())->getResourceId();
			assert(resId >= 0 && resId <= 3);
			if (resId >= 0) {
				++workersPerRes[resId];
			}
		}
	}

	auto vals = resources.getValues();
	for (int i = 0; i < vals.size(); ++i) {
		elements[i]->setText(
		                     Urho3D::String((int)vals[i]) + "|", Urho3D::String(workersPerRes[i]));
	}
}

void TopPanel::setVisible(bool enable) {
	SimplePanel::setVisible(enable);
	//if (!enable) {
	infoPanel->setVisible(enable);
	//}
}
