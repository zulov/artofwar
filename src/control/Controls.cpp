#include "Controls.h"
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/Log.h>
#include "ControlsUtils.h"
#include "SelectedInfo.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "commands/action/ResourceActionCommand.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "hud/HudData.h"
#include "math/MathUtils.h"
#include "objects/NodeUtils.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "objects/unit/order/GroupOrder.h"
#include "objects/unit/order/IndividualOrder.h"
#include "objects/unit/order/enums/UnitActionType.h"
#include "player/PlayersManager.h"
#include "player/ai/ActionCenter.h"
#include "simulation/ObjectsInfo.h"
#include "simulation/env/Environment.h"
#include "objects/ActionType.h"
#include "simulation/FrameInfo.h"
#include "simulation/SimInfo.h"
#include "utils/OtherUtils.h"

Controls::Controls(Urho3D::Input* _input): input(_input), typeToCreate(ObjectType::NONE) {
	if (!SIM_GLOBALS.HEADLESS) {
		selectedInfo = new SelectedInfo();
		selected.reserve(5000);
		selectionNode = createNode("Objects/additional/selectionNode.xml");
		arrowNode = createNode("Objects/additional/arrowNode.xml");

		tempBuildingNode = Game::getScene()->CreateChild();
		tempBuildingNode->SetEnabled(false);

		selectedInfo->setSelectedType(ObjectType::NONE);

		for (int i = 0; i < MAX_DEPLOY_MARK_NUMBER; ++i) {
			deployMark[i] = createNode("Objects/buildings/additional/banner.xml");
		}
	}
}

Controls::~Controls() {
	delete selectedInfo;
	if (!SIM_GLOBALS.HEADLESS) {
		selectionNode->Remove();
		arrowNode->Remove();

		for (auto mark : deployMark) {
			mark->Remove();
		}
	}
}

void Controls::init() {
	billboardSetProvider.init();
}

void Controls::updateAdditionalInfo() const {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::BUILDING: {
		int min = Urho3D::Min(MAX_DEPLOY_MARK_NUMBER, selected.size());
		for (int i = 0; i < min; ++i) {
			deployMark[i]->SetEnabled(true);
			auto deployIndex = static_cast<Building*>(selected.at(i))->getDeploy().value();
			deployMark[i]->SetPosition(Game::getEnvironment()->getPosWithHeightAt(deployIndex));
		}
		for (int i = min; i < MAX_DEPLOY_MARK_NUMBER; ++i) {
			deployMark[i]->SetEnabled(false);
		}
	}
	break;
	default:
		for (auto mark : deployMark) {
			mark->SetEnabled(false);
		}
	}
}

void Controls::unSelectAll() {
	for (auto& phy : selected) {
		phy->unSelect();
	}
	billboardSetProvider.reset();
	selected.clear();
	selectedInfo->setSelectedType(ObjectType::NONE);
	selectedInfo->reset();
}

void Controls::selectOne(Physical* entity, char player) {
	const auto entityType = entity->getType();
	if (entity == nullptr || entityType != selectedInfo->getSelectedType()) {
		unSelectAll();
	}
	if (!entity->isSelected() && entity->isAlive()
		&& (entity->getPlayer() < 0 || entity->getPlayer() == player)) {

		entity->select(billboardSetProvider.getNextBar(entity->getType(), entity->getPlayer()),
		               billboardSetProvider.getNextAura(entity->getType(), entity->getPlayer(), entity->getId()));

		selected.push_back(entity);

		selectedInfo->setSelectedType(entityType);
		selectedInfo->setAllNumber(selected.size());
		selectedInfo->select(entity);
	}
}

void Controls::select(const std::vector<Physical*>* entities) {
	auto player = Game::getPlayersMan()->getActivePlayerID();
	for (auto physical : *entities) {
		selectOne(physical, player);
	}
	billboardSetProvider.commit();
	updateAdditionalInfo();
}

void Controls::leftClick(hit_data& hitData) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	if (hitData.isGround) {
		unSelectAll();
	} else {
		selectOne(hitData.clicked, Game::getPlayersMan()->getActivePlayerID());
	}

	billboardSetProvider.commit();
	updateAdditionalInfo();
}

void Controls::leftDoubleClick(hit_data& hitData) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(Game::getEnvironment()->getNeighboursSimilarAs(hitData.clicked));
}

void Controls::leftClickBuild(hit_data& hitData) {
	unSelectAll();
	createBuilding({hitData.position.x_, hitData.position.z_});
}

UnitOrder* Controls::vectorOrder(UnitAction order, Urho3D::Vector2* vector, const bool shiftPressed,
                                 const std::vector<Physical*>& vec) const {
	if (vec.size() == 1) {
		return new IndividualOrder(static_cast<Unit*>(vec.at(0)), order, *vector, shiftPressed);
	}
	return new GroupOrder(vec, UnitActionType::ORDER, static_cast<short>(order), *vector, shiftPressed);
}

UnitOrder* Controls::thingOrder(UnitAction order, Physical* toUse, const bool shiftPressed,
                                const std::vector<Physical*>& vec) const {
	if (vec.size() == 1) {
		return new IndividualOrder(static_cast<Unit*>(vec.at(0)), order, toUse, shiftPressed);
	}
	return new GroupOrder(vec, UnitActionType::ORDER, static_cast<short>(order), toUse, shiftPressed);
}

UnitAction Controls::chooseAction(hit_data& hitData) const {
	return hitData.clicked->getTeam() == selected.at(0)->getTeam()
		       ? UnitAction::FOLLOW
		       : UnitAction::ATTACK;
}

UnitOrder* Controls::createUnitOrder(hit_data& hitData) const {
	const bool shiftPressed = input->GetKeyDown(Urho3D::KEY_SHIFT);
	switch (hitData.getType()) {
	case ObjectType::NONE:
		return vectorOrder(UnitAction::GO, new Urho3D::Vector2(hitData.position.x_, hitData.position.z_), shiftPressed,
		                   selected);
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		return thingOrder(chooseAction(hitData), hitData.clicked, shiftPressed, selected);
	case ObjectType::RESOURCE:
		return thingOrder(UnitAction::COLLECT, hitData.clicked, shiftPressed, selected);
	default: ;
	}
}

void Controls::rightClick(hit_data& hitData) const {
	Game::getActionCenter()->addUnitAction(createUnitOrder(hitData), Game::getPlayersMan()->getActivePlayerID());
}

void Controls::leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}

	select(Game::getEnvironment()->getNeighbours(held, Game::getPlayersMan()->getActivePlayerID()));
}

void Controls::rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const {
	GroupOrder* first = new GroupOrder(selected, UnitActionType::ORDER, cast(UnitAction::GO),
	                                   Urho3D::Vector2(held.first->x_, held.first->z_));
	GroupOrder* second;
	if (input->GetKeyDown(Urho3D::KEY_SHIFT)) {
		second = new GroupOrder(selected, UnitActionType::ORDER, cast(UnitAction::GO),
		                        Urho3D::Vector2(held.second->x_, held.second->z_), true);
	} else {
		second = new GroupOrder(selected, UnitActionType::ORDER, cast(UnitAction::CHARGE),
		                        Urho3D::Vector2(held.second->x_ - held.first->x_,
		                                        held.second->z_ - held.first->z_), true); //TODO buf append nie dzia³a
	}

	Game::getActionCenter()->addUnitAction(first, second, Game::getPlayersMan()->getActivePlayerID());
}

void Controls::releaseLeft() {
	hit_data hitData;

	if (raycast(hitData)) {
		const auto lastClicked = left.lastUp;
		left.setSecond(hitData.position);
		const float dist = sqDist(left.held.first, left.held.second);
		if (left.lastUp - lastClicked < 0.2f && dist < clickDistance) {
			leftDoubleClick(hitData);
		} else if (dist > clickDistance) {
			leftHold(left.held);
		} else if (hitData.isSth()) {
			leftClick(hitData);
		}
	}
	selectionNode->SetEnabled(false);
	left.clean();
}

void Controls::releaseRight() {
	hit_data hitData;

	if (selectedInfo->getSelectedType() == ObjectType::UNIT && raycast(hitData)) {
		right.setSecond(hitData.position);
		if (sqDist(right.held.first, right.held.second) > clickDistance) {
			rightHold(right.held);
		} else if (hitData.isSth()) {
			rightClick(hitData);
		}
	}
	arrowNode->SetEnabled(false);
	right.clean();
}

void Controls::releaseBuildLeft() {
	hit_data hitData;

	if (raycastGround(hitData)) {
		leftClickBuild(hitData);
		if (input->GetKeyDown(Urho3D::KEY_CTRL)) {
			cleanMouse();
		} else {
			toDefault();
		}
	}
}

bool Controls::orderAction() const {
	hit_data hitData;

	//if (raycast(hitData) && hitData.clicked) {
	if (raycast(hitData)) {
		rightClick(hitData);
		return true;
	}
	return false;
}

void Controls::toBuild(HudData* hud) {
	state = ControlsState::BUILD;
	typeToCreate = ObjectType::BUILDING;
	idToCreate = hud->getId();
	tempBuildingNode->SetEnabled(false);
}

BuildingActionType Controls::getBuildingActionType(ActionType type) {
	switch (type) {
	case ActionType::UNIT_CREATE:
		return BuildingActionType::UNIT_CREATE;
	case ActionType::UNIT_LEVEL:
		return BuildingActionType::UNIT_LEVEL;
	case ActionType::UNIT_UPGRADE:
		return BuildingActionType::UNIT_UPGRADE;
	default: ;
	}
}

void Controls::order(short id, ActionType type) {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::NONE:
		return Game::getActionCenter()->add(
			new GeneralActionCommand(id, GeneralActionType::BUILDING_LEVEL,
			                         Game::getPlayersMan()->getActivePlayerID()));
	case ObjectType::UNIT:
		return actionUnit(id, type);
	case ObjectType::BUILDING:
		return executeOnBuildings(getBuildingActionType(type), id);
	case ObjectType::RESOURCE:
		return executeOnResources(ResourceActionType(id));
	}
}

void Controls::executeOnUnits(short id) const {
	Game::getActionCenter()->addUnitAction(
		new GroupOrder(selected, UnitActionType::ORDER, id, nullptr, false),
		Game::getPlayersMan()->getActivePlayerID());
}

void Controls::executeOnResources(ResourceActionType action) const {
	Game::getActionCenter()->add(
		new ResourceActionCommand(selected, action, Game::getPlayersMan()->getActivePlayerID()));
}

void Controls::executeOnBuildings(BuildingActionType action, short id) const {
	Game::getActionCenter()->add(
		new BuildingActionCommand(selected, action, id, Game::getPlayersMan()->getActivePlayerID()));
}

bool Controls::clickDown(MouseButton& var) const {
	hit_data hitData;
	if (raycast(hitData)) {
		var.setFirst(hitData.position);
		return true;
	}
	return false;
}

void Controls::createBuilding(Urho3D::Vector2 pos) const {
	if (idToCreate >= 0) {
		auto player = Game::getPlayersMan()->getActivePlayerID();
		Game::getActionCenter()->addBuilding(idToCreate, pos, player);
	}
}

void Controls::cleanMouse() {
	left.clean();
	right.clean();
	selectionNode->SetEnabled(false);
	arrowNode->SetEnabled(false);
}

void Controls::deactivate() {
	if (active) {
		active = false;
		cleanMouse();
	}
}

void Controls::activate() {
	if (!active) {
		active = true;
		cleanMouse();
	}
}

void Controls::unitOrder(short id) {
	const auto type = UnitAction(id);
	switch (type) {
	case UnitAction::GO:
	case UnitAction::CHARGE:
	case UnitAction::ATTACK:
	case UnitAction::FOLLOW:
		state = ControlsState::ORDER;
		unitOrderType = type;
		break;
	case UnitAction::STOP:
	case UnitAction::DEFEND:
	case UnitAction::DEAD:
		executeOnUnits(id);
		break;
	default: ;
	}
}

void Controls::actionUnit(short id, ActionType type) {
	switch (type) {
	case ActionType::ORDER:
		return unitOrder(id);
	case ActionType::FORMATION:
		return unitFormation(id);
	}
}

void Controls::refreshSelected() {
	bool sizeBefore = selected.size();
	selected.erase(
		std::remove_if(
			selected.begin(), selected.end(),
			[](Physical* physical) {
				if (!physical->isAlive()) {
					physical->unSelect();
					return true;
				}
				return false;
			}),
		selected.end());

	bool sizeAfter = selected.size();
	if (sizeBefore != sizeAfter) {
		//TODO perf nie koniecznie resetowac ca³oœæ
		selectedInfo->refresh(selected);
	}
	//TODO bug billboard beda zajete
}

bool Controls::conditionToClean(const SimInfo* simulationInfo) const {
	if (!simulationInfo->getFrameInfo()->isRealFrame()) {
		return false;
	}
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::UNIT:
		return simulationInfo->getObjectsInfo()->ifUnitDied();
	case ObjectType::BUILDING:
		return simulationInfo->getObjectsInfo()->ifBuildingDied();
	case ObjectType::RESOURCE:
		return simulationInfo->getObjectsInfo()->ifResourceDied();
	}
	return false;
}

void Controls::cleanAndUpdate(const SimInfo* simulationInfo) {
	if (conditionToClean(simulationInfo)) {
		refreshSelected();
	}
	for (auto physical : selected) {
		physical->updateBillboards();
	}
	billboardSetProvider.commit();
}

void Controls::updateSelection() const {
	hit_data hitData;

	if (raycastGround(hitData)) {
		const float xScale = left.held.first->x_ - hitData.position.x_;
		const float zScale = left.held.first->z_ - hitData.position.z_;
		if ((xScale * xScale > clickDistance || zScale * zScale > clickDistance)
			&& Game::getTime() - left.lastDown > 0.3f) {
			if (!selectionNode->IsEnabled()) {
				selectionNode->SetEnabled(true);
			}
			selectionNode->SetScale({abs(xScale), 0.1f, abs(zScale)});
			selectionNode->SetPosition((*left.held.first + hitData.position) / 2);
		} else {
			if (selectionNode->IsEnabled()) {
				selectionNode->SetEnabled(false);
			}
		}
	}
}

void Controls::updateArrow() const {
	hit_data hitData;

	if (selectedInfo->getSelectedType() == ObjectType::UNIT && raycastGround(hitData)) {
		auto dir = *right.held.first - hitData.position;

		const float length = dir.Length();

		if (length * length > clickDistance && Game::getTime() - right.lastDown > 0.3f) {
			if (!arrowNode->IsEnabled()) {
				arrowNode->SetEnabled(true);
			}
			arrowNode->SetScale({length, 1, length / 3});
			arrowNode->SetDirection({-dir.z_, 0, dir.x_});
			arrowNode->SetPosition(*right.held.first);
		} else if (arrowNode->IsEnabled()) {
			arrowNode->SetEnabled(false);
		}
	} else if (arrowNode->IsEnabled()) {
		arrowNode->SetEnabled(false);
	}
}

void Controls::toDefault() {
	state = ControlsState::DEFAULT;
	cleanMouse();
	idToCreate = -1;
	tempBuildingNode->SetEnabled(false);
}

void Controls::unitFormation(short id) const {
	Game::getActionCenter()->addUnitAction(
		new GroupOrder(selected, UnitActionType::FORMATION, id, nullptr), Game::getPlayersMan()->getActivePlayerID());
}

void Controls::control() {
	switch (state) {
	case ControlsState::DEFAULT:
		return defaultControl(); //TODO bug gdy zaznaczony jest resource to probuje storzyc formacje przy chargfe
	case ControlsState::BUILD:
		return buildControl();
	case ControlsState::ORDER:
		return orderControl();
	}
}

void Controls::defaultControl() {
	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		if (!left.isHeld) {
			clickDown(left);
		} else {
			updateSelection();
		}
	} else if (left.isHeld) {
		releaseLeft();
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		if (!right.isHeld) {
			clickDown(right);
		} else {
			updateArrow();
		}
	} else if (right.isHeld) {
		releaseRight();
	}

	if (input->GetMouseButtonPress(Urho3D::MOUSEB_MIDDLE)) {
		hit_data hitData;

		if (raycast(hitData)) {
			if (hitData.clicked) {
				Game::getLog()->Write(3, hitData.clicked->toMultiLineString());
			}
			Game::getLog()->Write(3, hitData.position.ToString());
		}
	}
}

void Controls::buildControl() {
	if (!input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT) && !input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		hit_data hitData;

		if (raycastGround(hitData)) {
			//TODO perf nie robic tego co klatkê
			auto env = Game::getEnvironment();

			const auto building = Game::getDatabase()->getBuilding(idToCreate);

			auto hitPos = Urho3D::Vector2(hitData.position.x_, hitData.position.z_);

			const auto validPos = env->getValidPosition(building->size, hitPos);

			tempBuildingNode->SetPosition(env->getPosWithHeightAt(validPos.x_, validPos.y_));
			if (!tempBuildingNode->IsEnabled()) {

				auto level = building->getLevel(0).value();
				tempBuildingNode->LoadXML(Game::getCache()
				                          ->GetResource<Urho3D::XMLFile>("Objects/buildings/" + level->nodeName)->
				                          GetRoot());
				tempBuildingModel = tempBuildingNode->GetComponent<Urho3D::StaticModel>();
				tempBuildingNode->SetEnabled(true);
			}
			Urho3D::String textureName;
			if (env->validateStatic(building->size, hitPos) && env->isVisible(Game::getPlayersMan()->getActivePlayerID(), hitPos)) {
				textureName = "Materials/green_overlay.xml";
			} else {
				textureName = "Materials/red_overlay.xml";

			}
			changeMaterial(Game::getCache()->GetResource<Urho3D::Material>(textureName), tempBuildingModel);
		}
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		left.markIfNotHeld();
	} else if (left.isHeld) {
		releaseBuildLeft();
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		toDefault();
	}
}

void Controls::orderControl() {
	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		left.markIfNotHeld();
	} else if (left.isHeld) {
		switch (unitOrderType) {
		case UnitAction::GO:
		case UnitAction::ATTACK:
		case UnitAction::FOLLOW:
			if (orderAction()) {
				toDefault();
			}
			break;
		case UnitAction::CHARGE:
			break;
		default: ;
		}
		left.clean();
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		right.markIfNotHeld();
	} else if (right.isHeld) {
		toDefault();
	}
}
