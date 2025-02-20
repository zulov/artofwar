#include "TopPanel.h"
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Resource/Localization.h>
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
#include "simulation/FrameInfo.h"

const Urho3D::String monthsRoman[] = {
	"   I", "  II", " III", "  IV", "   V", "  VI", " VII", "VIII", "  IX", "   X", "  XI", " XII"
};

TopPanel::TopPanel(Urho3D::UIElement* root, Urho3D::XMLFile* style) : SimplePanel(root, style, "TopWindow",
	{GameState::RUNNING, GameState::PAUSE}) {
}


TopPanel::~TopPanel() {
	clear_array(elements, RESOURCES_SIZE);
	delete units;
	delete workers;
	delete name;
	delete infoPanel;
	delete time;
}

void TopPanel::createBody() {
	const Urho3D::String path = "textures/hud/icon/top/";

	name = new TopHudElement(window, style, getTexture(path + "helmet.png"));
	time = new TopHudElement(window, style, getTexture(path + "time.png"), false, "TopButtonsWide");
	units = new TopHudElement(window, style, getTexture(path + "human.png"), "TopButtonsNarrow");
	workers = new TopHudElement(window, style, getTexture(path + "worker.png"), "TopButtonsNarrow");

	const Urho3D::String resIconPath = "textures/hud/icon/resource/";
	elements[0] = new TopHudElement(window, style, getTexture(resIconPath + "food.png"));
	elements[1] = new TopHudElement(window, style, getTexture(resIconPath + "wood.png"));
	elements[2] = new TopHudElement(window, style, getTexture(resIconPath + "stone.png"));
	elements[3] = new TopHudElement(window, style, getTexture(resIconPath + "gold.png"));


	infoPanel = new TopInfoPanel(root, style);
	infoPanel->createWindow();
	infoPanel->setVisible(true);
}

void TopPanel::update(Player* player, FrameInfo* frameInfo) const {
	auto poss = player->getPossession();

	name->setText(player->getName(), Urho3D::String((int)player->getId()))
	    ->setToolTip(l10nFormat("top_name_tooltip", (int)player->getTeam()));

	auto freeArmy = Urho3D::String(poss->getFreeArmyNumber());
	auto wholeArmy = Urho3D::String(poss->getArmyNumber());
	units->setText(freeArmy, wholeArmy)
	     ->setToolTip(l10nFormat("top_units_tooltip", poss->getFreeArmyNumber(), poss->getArmyNumber(),
	                             poss->getInfantryNumber(), poss->getCalvaryNumber(),
	                             poss->getMeleeNumber(), poss->getRangeNumber(),
	                             poss->getLightNumber(), poss->getHeavyNumber()));

	auto [month, year] = frameInfo->getDate();
	auto [h, m, s] = frameInfo->getTime();

	time->setText(monthsRoman[month] + " " + Urho3D::String(year),
	              Urho3D::String((int)h) + ":" + to2DigString(m) + ":" + to2DigString(s));

	unsigned short workersPerRes[] = {0, 0, 0, 0};
	for (const auto worker : poss->getWorkers()) {
		if (worker->getState() == UnitState::COLLECT) {
			//TODO error thing to interact nie jest zapisywany!
			const auto resId = dynamic_cast<ResourceEntity*>(worker->getThingToInteract())->getResourceId();
			assert(resId >= 0 && resId <= 3);
			if (resId >= 0) {
				++workersPerRes[resId];
			}
		}
	}

	workers->setText(Urho3D::String(poss->getFreeWorkersNumber()), Urho3D::String(poss->getWorkersNumber()))
	       ->setToolTip(l10nFormat("top_workers_tooltip", workersPerRes[0], workersPerRes[1], workersPerRes[2],
	                               workersPerRes[3]));

	const auto& resources = player->getResources();
	auto vals = resources->getValues();
	for (int i = 0; i < vals.size(); ++i) {
		elements[i]->setText(Urho3D::String((int)vals[i]), Urho3D::String(workersPerRes[i]));
	}
	auto gather = resources->getGatherSpeeds();
	auto without = poss->getResWithOutBonus();

	elements[0]->setToolTip(l10nFormat("top_food_tooltip",
	                                   asStringF(gather[0], 1).c_str(),
	                                   (int)(workersPerRes[0] - without[0]),
	                                   workersPerRes[0],
	                                   (int)vals[0],
	                                   (int)resources->getFoodStorage(),
	                                   (int)resources->getLastFoodLost(),
	                                   (int)resources->potentialFoodLost()));
	elements[1]->setToolTip(l10nFormat("top_wood_tooltip",
	                                   asStringF(gather[1], 1).c_str(),
	                                   (int)(workersPerRes[1] - without[1]),
	                                   workersPerRes[1]));
	elements[2]->setToolTip(l10nFormat("top_stone_tooltip",
	                                   asStringF(gather[2], 1).c_str(),
	                                   (int)(workersPerRes[2] - without[2]),
	                                   workersPerRes[2],
	                                   (int)resources->getStoneRefineCapacity(),
	                                   (int)resources->getPotentialStoneRefinement()));
	elements[3]->setToolTip(l10nFormat("top_gold_tooltip",
	                                   asStringF(gather[3], 1).c_str(),
	                                   (int)(workersPerRes[3] - without[3]), workersPerRes[3],
	                                   (int)vals[3], (int)resources->getGoldStorage(),
	                                   (int)resources->getLastGoldGains(), (int)resources->getPotentialGoldGains(),
	                                   (int)resources->getGoldRefineCapacity(),
	                                   (int)resources->getPotentialGoldRefinement()));
}

void TopPanel::setVisible(bool enable) {
	SimplePanel::setVisible(enable);
	//if (!enable) {
	infoPanel->setVisible(enable);
	//}
}
