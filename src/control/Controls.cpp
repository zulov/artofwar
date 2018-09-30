#include "Controls.h"
#include "ControlsUtils.h"
#include "Game.h"
#include "HitData.h"
#include "MathUtils.h"
#include "ObjectEnums.h"
#include "camera/CameraManager.h"
#include "commands/CommandList.h"
#include "commands/action/ActionCommand.h"
#include "commands/action/GroupAction.h"
#include "commands/creation/CreationCommandList.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include "objects/MenuAction.h"
#include "objects/NodeUtils.h"
#include "objects/queue/QueueManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "simulation/SimulationInfo.h"
#include "simulation/env/Enviroment.h"
#include "simulation/formation/FormationManager.h"
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <algorithm>
#include <queue>


Controls::Controls(Urho3D::Input* _input): typeToCreate(ObjectType::ENTITY), input(_input) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	selectedInfo = new SelectedInfo();

	createNode("Models/box.mdl", "Materials/green_overlay.xml", &selectionNode);
	createNode("Models/arrow2.mdl", "Materials/red_overlay.xml", &arrowNode);

	tempBuildingNode = Game::getScene()->CreateChild();
	tempBuildingModel = tempBuildingNode->CreateComponent<Urho3D::StaticModel>();
	tempBuildingNode->SetEnabled(false);

	selectedInfo->setSelectedType(ObjectType::PHYSICAL);
}

Controls::~Controls() {
	delete selectedInfo;
	delete selected;
	selectionNode->Remove();
	arrowNode->Remove();
}


void Controls::unSelectAll() const {
	for (auto& phy : *selected) {
		phy->unSelect();
	}
	selected->clear();
	selectedInfo->setSelectedType(ObjectType::ENTITY);
	selectedInfo->reset();
}

void Controls::selectOne(Physical* entity) const {
	const auto entityType = entity->getType();
	if (entityType != selectedInfo->getSelectedType()) {
		unSelectAll();
	}
	if (!entity->isSelected()) {
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
}

void Controls::select(Physical* physical) const {
	selectOne(physical);
}

void Controls::leftClick(hit_data& hitData) const {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(hitData.clicked);
}

void Controls::leftClickBuild(hit_data& hitData) {
	unSelectAll();
	createBuilding({hitData.position.x_, hitData.position.z_});
}

void Controls::rightClick(hit_data& hitData) const {
	switch (hitData.clicked->getType()) {
	case ObjectType::PHYSICAL:
		{
		Game::getActionList()->add(new GroupAction(selected, UnitOrder::GO,
		                                           new Urho3D::Vector2(hitData.position.x_, hitData.position.z_),
		                                           input->GetKeyDown(Urho3D::KEY_SHIFT)));
		break;
		}
	case ObjectType::UNIT:
	case ObjectType::BUILDING:
	case ObjectType::RESOURCE:
		Game::getActionList()->add(new GroupAction(selected, UnitOrder::FOLLOW, hitData.clicked,
		                                           input->GetKeyDown(Urho3D::KEY_SHIFT)));
		break;
	default: ;
	}
}

void Controls::leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(Game::getEnviroment()->getNeighbours(held));
}

void Controls::rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) const {
	auto actions = Game::getActionList();

	const auto first = new Urho3D::Vector2(held.first->x_, held.first->z_);
	if (input->GetKeyDown(Urho3D::KEY_SHIFT)) {
		const auto second = new Urho3D::Vector2(held.second->x_, held.second->z_);
		actions->add(new GroupAction(selected, UnitOrder::GO, first));
		actions->add(new GroupAction(selected, UnitOrder::GO, second, true));
	} else {
		const auto charge = new Urho3D::Vector2(held.second->x_ - held.first->x_, held.second->z_ - held.first->z_);

		actions->add(new GroupAction(selected, UnitOrder::GO, first));
		actions->add(new GroupAction(selected, UnitOrder::CHARGE, charge, true));
	}
}

void Controls::releaseLeft() {
	hit_data hitData;

	if (raycast(hitData)) {
		left.setSecond(hitData.position);
		if (sqDist(left.held.first, left.held.second) > clickDistance) {
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

bool Controls::orderAction() {
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
		return executeOnAll(id, parameter);
	case ObjectType::RESOURCE:
		return executeOnAll(id, parameter);
	}
}

void Controls::executeOnAll(short id, const ActionParameter& parameter) {
	for (auto& phy : *selected) {
		phy->action(id, parameter);
	}
}

void Controls::orderPhysical(short id, const ActionParameter& parameter) const {
	switch (parameter.type) {
	case MenuAction::BUILDING_LEVEL:
		{
		const auto level = Game::getPlayersManager()->getActivePlayer()->getLevelForBuilding(id) + 1;
		auto opt = Game::getDatabaseCache()->getCostForBuildingLevel(id, level);
		if (opt.has_value()) {
			auto& resources = Game::getPlayersManager()->getActivePlayer()->getResources();
			if (resources.reduce(opt.value())) {
				Game::getQueueManager()->add(1, parameter.type, id, 1);
			}
		}
		break;
		}
	}
}

bool Controls::clickDown(MouseButton& var) {
	hit_data hitData;
	if (raycast(hitData)) {
		var.setFirst(hitData.position);
		return true;
	}
	return false;
}

void Controls::createBuilding(Urho3D::Vector2 pos) {
	if (idToCreate >= 0) {
		auto player = Game::getPlayersManager()->getActivePlayer();

		Game::getCreationList()->addBuilding(idToCreate, pos,
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
		executeOnAll(id, ActionParameter());
		break;
	default: ;
	}
}

void Controls::actionUnit(short id, const ActionParameter& parameter) {
	switch (parameter.type) {
	case MenuAction::ORDER:
		return unitOrder(id);
	case MenuAction::FORMATION:
		return unitFormation(id);
	}
}

void Controls::refreshSelected() {
	selected->erase(
	                std::remove_if(
	                               selected->begin(), selected->end(),
	                               [](Physical* physical)
	                               {
		                               if (!physical->isAlive()) {
			                               physical->unSelect();
			                               return true;
		                               }
		                               return false;
	                               }),
	                selected->end());
}

bool Controls::conditionToClean(SimulationInfo* simulationInfo) {
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

void Controls::clean(SimulationInfo* simulationInfo) {
	if (conditionToClean(simulationInfo)) {
		refreshSelected();
	}
}

void Controls::updateSelection() {
	hit_data hitData;

	if (raycast(hitData, ObjectType::PHYSICAL)) {
		const float xScale = left.held.first->x_ - hitData.position.x_;
		const float zScale = left.held.first->z_ - hitData.position.z_;
		if (xScale * xScale > clickDistance || zScale * zScale > clickDistance) {
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

void Controls::updateArrow() {
	hit_data hitData;

	if (selectedInfo->getSelectedType() == ObjectType::UNIT && raycast(hitData, ObjectType::PHYSICAL)) {
		auto dir = *right.held.first - hitData.position;

		const float length = dir.Length();
		if (length * length > clickDistance) {
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

void Controls::unitFormation(short id) {
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
			selectedInfo->setMessage(hitData.position.ToString());
		}
	}
}

void Controls::buildControl() {
	if (!input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT) && !input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		hit_data hitData;

		if (raycast(hitData, ObjectType::PHYSICAL)) {
			//TODO perf nie robic tego co klatkê
			auto env = Game::getEnviroment();

			const auto dbBuilding = Game::getDatabaseCache()->getBuilding(idToCreate);
			auto dbLevel = Game::getDatabaseCache()->getBuildingLevel(dbBuilding->id, 0).value();
			auto hitPos = Urho3D::Vector2(hitData.position.x_, hitData.position.z_);

			const auto validPos = env->getValidPosition(dbBuilding->size, hitPos);
			const auto height = env->getGroundHeightAt(validPos.x_, validPos.y_);

			tempBuildingNode->SetPosition(Urho3D::Vector3(validPos.x_, height, validPos.y_));
			if (!tempBuildingNode->IsEnabled()) {
				tempBuildingNode->SetScale(dbLevel->scale);

				tempBuildingModel->SetModel(Game::getCache()->GetResource<Urho3D::Model>("Models/" + dbLevel->model));
				tempBuildingNode->SetEnabled(true);
			}
			if (env->validateStatic(dbBuilding->size, hitPos)) {
				changeMaterial(Game::getCache()->GetResource<Urho3D::Material>("Materials/green_overlay.xml"),
				               tempBuildingModel);
			} else {
				changeMaterial(Game::getCache()->GetResource<Urho3D::Material>("Materials/red_overlay.xml"),
				               tempBuildingModel);
			}

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
