#include "Controls.h"
#include "ControlsUtils.h"
#include "Game.h"
#include "HitData.h"
#include "MathUtils.h"
#include "objects/ObjectEnums.h"
#include "camera/CameraManager.h"
#include "commands/action/ActionCommand.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include "objects/ActionType.h"
#include "objects/NodeUtils.h"
#include "objects/queue/QueueManager.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/Unit.h"
#include "objects/unit/aim/order/FutureOrder.h"
#include "objects/unit/aim/order/GroupOrder.h"
#include "objects/unit/aim/order/IndividualOrder.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/SimulationInfo.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/Log.h>
#include <algorithm>
#include <queue>
#include "ActionCenter.h"


Controls::Controls(Urho3D::Input* _input): input(_input), typeToCreate(ObjectType::NONE) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	selectedInfo = new SelectedInfo();

	createNode("Models/box.mdl", "Materials/green_overlay.xml", &selectionNode);
	createNode("Models/arrow2.mdl", "Materials/red_overlay.xml", &arrowNode);

	tempBuildingNode = Game::getScene()->CreateChild();
	tempBuildingNode->SetEnabled(false);

	selectedInfo->setSelectedType(ObjectType::PHYSICAL);

	for (int i = 0; i < MAX_DEPLOY_MARK_NUMBER; ++i) {
		deployMark[i] = Game::getScene()->CreateChild();
		deployMark[i]->LoadXML(Game::getCache()->GetResource<Urho3D::XMLFile>
			("Objects/buildings/additional/target.xml")->GetRoot());
	}
}

Controls::~Controls() {
	delete selectedInfo;
	delete selected;
	selectionNode->Remove();
	arrowNode->Remove();

	for (auto mark : deployMark) {
		mark->Remove();
	}
}

void Controls::updateAdditionalInfo() const {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::BUILDING:
	{
		int min = Urho3D::Min(MAX_DEPLOY_MARK_NUMBER, selected->size());
		for (int i = 0; i < min; ++i) {
			deployMark[i]->SetEnabled(true);
			auto deployIndex = selected->at(i)->getDeploy().value();
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


void Controls::unSelectAll() const {
	for (auto& phy : *selected) {
		phy->unSelect();
	}
	selected->clear();
	selectedInfo->setSelectedType(ObjectType::NONE);
	selectedInfo->reset();
}

void Controls::selectOne(Physical* entity) const {
	const auto entityType = entity->getType();
	if (entityType != selectedInfo->getSelectedType()) {
		unSelectAll();
	}
	if (!entity->isSelected() && entity->isAlive()) {
		entity->select();
		selected->push_back(entity);

		selectedInfo->setSelectedType(entityType);
		selectedInfo->setAllNumber(selected->size());
		selectedInfo->select(entity);
	}
}

void Controls::select(std::vector<Physical*>* entities) const {
	for (auto physical : *entities) {
		selectOne(physical); //TODO perf zastapic wrzuceniem na raz
	}
	updateAdditionalInfo();
}

void Controls::leftClick(hit_data& hitData) const {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	selectOne(hitData.clicked);
	updateAdditionalInfo();
}

void Controls::leftDoubleClick(hit_data& hitData) const {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(Game::getEnvironment()->getNeighboursSimilarAs(hitData.clicked));
}

void Controls::leftClickBuild(hit_data& hitData) const {
	unSelectAll();
	createBuilding({hitData.position.x_, hitData.position.z_});
}

void Controls::rightClick(hit_data& hitData) const {
	UnitOrder order;
	Urho3D::Vector2 vector;
	Physical* toUse;
	switch (hitData.clicked->getType()) {
	case ObjectType::PHYSICAL:
		order = UnitOrder::GO;
		vector = {hitData.position.x_, hitData.position.z_};
		toUse = nullptr;
		break;
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
		order = hitData.clicked->getTeam() == selected->at(0)->getTeam()
			        ? UnitOrder::FOLLOW
			        : UnitOrder::ATTACK;
		vector = {};
		toUse = hitData.clicked;
		break;
	case ObjectType::RESOURCE:
		order = UnitOrder::COLLECT;
		vector = {};
		toUse = hitData.clicked;
		break;
	default: ;
	}
	const bool shiftPressed = input->GetKeyDown(Urho3D::KEY_SHIFT);

	FutureOrder* fOrder;
	if (selected->size() == 1) {
		fOrder = new IndividualOrder(static_cast<Unit*>(selected->at(0)),
		                             order, vector, toUse, shiftPressed);
	} else {
		fOrder = new GroupOrder(selected, order, vector, toUse,
		                        ActionType::ORDER, shiftPressed);
	}

	Game::getActionCenter()->add(new ActionCommand(fOrder));
}

void Controls::leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(Game::getEnvironment()->getNeighbours(held));
}

void Controls::rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const {
	if (input->GetKeyDown(Urho3D::KEY_SHIFT)) {
		Game::getActionCenter()->add(
			new ActionCommand(new GroupOrder(selected, UnitOrder::GO,
			                                 {held.first->x_, held.first->z_}, nullptr, ActionType::ORDER)),
			new ActionCommand(new GroupOrder(selected, UnitOrder::GO,
			                                 {held.second->x_, held.second->z_},
			                                 nullptr, ActionType::ORDER, true)));
	} else {
		Game::getActionCenter()->add(new ActionCommand(new GroupOrder(selected, UnitOrder::GO,
		                                                              {held.first->x_, held.first->z_},
		                                                              nullptr, ActionType::ORDER)),
		                             new ActionCommand(new GroupOrder(selected, UnitOrder::CHARGE, {
			                                                              held.second->x_ - held.first->x_,
			                                                              held.second->z_ - held.first->z_
		                                                              }, nullptr, ActionType::ORDER, true)));
	}
}

void Controls::releaseLeft() {
	hit_data hitData;

	if (raycast(hitData)) {
		auto lastClicked = left.lastUp;
		left.setSecond(hitData.position);
		const float dist = sqDist(left.held.first, left.held.second);
		if (left.lastUp - lastClicked < 0.2 && dist < clickDistance) {
			leftDoubleClick(hitData);
		} else if (dist > clickDistance) {
			leftHold(left.held);
		} else if (hitData.clicked) {
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
		} else if (hitData.clicked) {
			rightClick(hitData);
		}
	}
	arrowNode->SetEnabled(false);
	right.clean();
}

void Controls::releaseBuildLeft() {
	hit_data hitData;

	if (raycast(hitData) && hitData.clicked) {
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

	if (raycast(hitData) && hitData.clicked) {
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

void Controls::order(short id, const ActionParameter& parameter) {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::PHYSICAL:
		return orderPhysical(id, parameter);
	case ObjectType::UNIT:
		return actionUnit(id, parameter);
	case ObjectType::BUILDING:
	case ObjectType::RESOURCE:
		return executeOnAll(id, parameter);
	}
}

void Controls::executeOnAll(short id, const ActionParameter& parameter) const {
	Game::getActionCenter()->add(
		new ActionCommand(new GroupOrder(selected, UnitOrder(id), {}, nullptr, parameter.type)));
	//TODO przyjrzec sie typowi
}

void Controls::orderPhysical(short id, const ActionParameter& parameter) const {
	switch (parameter.type) {
	case ActionType::BUILDING_LEVEL:
	{
		const auto level = Game::getPlayersMan()->getActivePlayer()->getLevelForBuilding(id) + 1;
		auto opt = Game::getDatabaseCache()->getCostForBuildingLevel(id, level);
		if (opt.has_value()) {
			if (Game::getPlayersMan()->getActivePlayer()->getResources().reduce(opt.value())) {
				Game::getQueueManager()->add(1, parameter.type, id, 1);
			}
		}
		break;
	}
	}
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
		auto player = Game::getPlayersMan()->getActivePlayer();

		Game::getActionCenter()->addBuilding(idToCreate, pos,
		                                     player->getId(),
		                                     player->getLevelForBuilding(idToCreate));
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
	const auto type = UnitOrder(id);
	switch (type) {
	case UnitOrder::GO:
	case UnitOrder::CHARGE:
	case UnitOrder::ATTACK:
	case UnitOrder::FOLLOW:
		state = ControlsState::ORDER;
		unitOrderType = type;
		break;
	case UnitOrder::STOP:
	case UnitOrder::DEFEND:
	case UnitOrder::DEAD:
		executeOnAll(id, ActionParameter::Builder().setType(ActionType::ORDER).build());
		break;
	default: ;
	}
}

void Controls::actionUnit(short id, const ActionParameter& parameter) {
	switch (parameter.type) {
	case ActionType::ORDER:
		return unitOrder(id);
	case ActionType::FORMATION:
		return unitFormation(id);
	}
}

void Controls::refreshSelected() const {
	bool sizeBefore = selected->size();
	selected->erase(
		std::remove_if(
			selected->begin(), selected->end(),
			[](Physical* physical) {
				if (!physical->isAlive()) {
					physical->unSelect();
					return true;
				}
				return false;
			}),
		selected->end());

	bool sizeAfter = selected->size();
	if (sizeBefore != sizeAfter) {
		//TODO perf nie koniecznie resetowac ca³oœæ
		selectedInfo->refresh(selected);
	}
}

bool Controls::conditionToClean(SimulationInfo* simulationInfo) const {
	switch (selectedInfo->getSelectedType()) {
	case ObjectType::UNIT:
		return simulationInfo->ifUnitDied();
	case ObjectType::BUILDING:
		return simulationInfo->ifBuildingDied();
	case ObjectType::RESOURCE:
		return simulationInfo->ifResourceDied();
	}
	return false;
}

void Controls::clean(SimulationInfo* simulationInfo) const {
	if (conditionToClean(simulationInfo)) {
		refreshSelected();
	}
}

void Controls::updateSelection() const {
	hit_data hitData;

	if (raycast(hitData, ObjectType::PHYSICAL)) {
		const float xScale = left.held.first->x_ - hitData.position.x_;
		const float zScale = left.held.first->z_ - hitData.position.z_;
		if ((xScale * xScale > clickDistance || zScale * zScale > clickDistance) && Game::getTime() - left.lastDown >
			0.3) {
			if (!selectionNode->IsEnabled()) {
				selectionNode->SetEnabled(true);
			}
			selectionNode->SetScale({abs(xScale), 0.1, abs(zScale)});
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

	if (selectedInfo->getSelectedType() == ObjectType::UNIT && raycast(hitData, ObjectType::PHYSICAL)) {
		auto dir = *right.held.first - hitData.position;

		const float length = dir.Length();

		if (length * length > clickDistance && Game::getTime() - right.lastDown > 0.3) {
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
	Game::getFormationManager()->createFormation(selected, FormationType(id));
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

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_MIDDLE)) {
		hit_data hitData;

		if (raycast(hitData)) {
			Game::getLog()->Write(0, hitData.position.ToString());
		}
	}
}

void Controls::buildControl() {
	if (!input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT) && !input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		hit_data hitData;

		if (raycast(hitData, ObjectType::PHYSICAL)) {
			//TODO perf nie robic tego co klatkê
			auto env = Game::getEnvironment();

			const auto dbBuilding = Game::getDatabaseCache()->getBuilding(idToCreate);

			auto hitPos = Urho3D::Vector2(hitData.position.x_, hitData.position.z_);

			const auto validPos = env->getValidPosition(dbBuilding->size, hitPos);

			tempBuildingNode->SetPosition(env->getPosWithHeightAt(validPos.x_, validPos.y_));
			if (!tempBuildingNode->IsEnabled()) {
				auto dbLevel = Game::getDatabaseCache()->getBuildingLevel(dbBuilding->id, 0).value();
				tempBuildingNode->LoadXML(Game::getCache()
				                          ->GetResource<Urho3D::XMLFile>("Objects/buildings/" + dbLevel->nodeName)->
				                          GetRoot());
				tempBuildingModel = tempBuildingNode->GetComponent<Urho3D::StaticModel>();
				tempBuildingNode->SetEnabled(true);
			}
			Urho3D::String textureName;
			if (env->validateStatic(dbBuilding->size, hitPos)) {
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
		case UnitOrder::GO:
		case UnitOrder::ATTACK:
		case UnitOrder::FOLLOW:
			if (orderAction()) {
				toDefault();
			}
			break;
		case UnitOrder::CHARGE:
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
