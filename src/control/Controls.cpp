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


Controls::Controls(Urho3D::Input* _input) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	input = _input;
	selectedInfo = new SelectedInfo();

	createNode("Models/box.mdl", "Materials/green_overlay.xml", &selectionNode);
	createNode("Models/arrow2.mdl", "Materials/red_overlay.xml", &arrowNode);

	tempBuildingNode = Game::getScene()->CreateChild();
	tempBuildingModel = tempBuildingNode->CreateComponent<Urho3D::StaticModel>();
	tempBuildingNode->SetEnabled(false);

	selectedType = ObjectType::PHYSICAL;
}


Controls::~Controls() {
	delete selectedInfo;
	delete selected;
	selectionNode->Remove();
	arrowNode->Remove();
}

void Controls::unSelectAll() {
	for (auto& phy : *selected) {
		phy->unSelect();
	}
	selected->clear();
	selectedType = ObjectType::ENTITY;
	selectedInfo->setSelectedType(selectedType);
	selectedInfo->reset();
}

void Controls::select(Physical* entity) {
	const auto entityType = entity->getType();
	if (entityType != selectedType) {
		unSelectAll();
	}

	entity->select();
	selected->push_back(entity);

	selectedType = entityType;
	selectedInfo->setSelectedType(selectedType);
	selectedInfo->setAllNumber(selected->size());
	selectedInfo->select(entity);
}

void Controls::leftClick(hit_data& hitData) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	select(hitData.link->getPhysical());
}

void Controls::leftClickBuild(hit_data& hitData) {
	unSelectAll();
	createBuilding({hitData.position.x_, hitData.position.z_});
}

void Controls::rightClickDefault(hit_data& hitData, bool shiftPressed) {
	switch (hitData.link->getPhysical()->getType()) {
	case ObjectType::PHYSICAL:
		{
		Game::getActionCommandList()->add(new GroupAction(selected, UnitOrder::GO,
		                                                  new Urho3D::Vector2(hitData.position.x_, hitData.position.z_),
		                                                  shiftPressed));
		break;
		}
	case ObjectType::UNIT:
		{
		Game::getActionCommandList()->add(new GroupAction(selected, UnitOrder::FOLLOW, hitData.link->getPhysical(),
		                                                  shiftPressed));
		//unSelectAll();
		break;
		}
	case ObjectType::BUILDING:
		Game::getActionCommandList()->add(new GroupAction(selected, UnitOrder::FOLLOW, hitData.link->getPhysical(),
		                                                  shiftPressed));
		break;
	case ObjectType::RESOURCE: 
		
		break;
	default: ;
	}
}

void Controls::leftHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) {
	if (!input->GetKeyDown(Urho3D::KEY_CTRL)) {
		unSelectAll();
	}
	auto entities = Game::getEnviroment()->getNeighbours(held);
	for (auto entity : *entities) {
		select(entity); //TODO zastapic wrzuceniem na raz
	}
}

void Controls::rightHold(std::pair<Urho3D::Vector3*, Urho3D::Vector3*>& held) {
	auto actions = Game::getActionCommandList();

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
		const float dist = sqDist(left.held.first, left.held.second);
		if (dist > clickDistance) {
			leftHold(left.held);
		} else {
			if (hitData.link) {
				leftClick(hitData);
			}
		}
		selectionNode->SetEnabled(false);
		left.clean();
	}
}

void Controls::releaseBuildLeft() {
	hit_data hitData;

	if (raycast(hitData)) {
		if (hitData.link) {
			leftClickBuild(hitData);
		}
	}
}

void Controls::releaseRight() {
	hit_data hitData;

	if (raycast(hitData)) {
		right.setSecond(hitData.position);
		if (sqDist(right.held.first, right.held.second) > clickDistance) {
			rightHold(right.held);
		} else {
			if (hitData.link) {
				rightClickDefault(hitData, input->GetKeyDown(Urho3D::KEY_SHIFT));
			}
		}
		arrowNode->SetEnabled(false);
		right.clean();
	}
}

bool Controls::orderAction(bool shiftPressed) {
	hit_data hitData;

	if (raycast(hitData) && hitData.link) {
		rightClickDefault(hitData, shiftPressed);
		return true;
	}
	return false;
}

void Controls::resetState() {
	if (selectedInfo->hasChanged()) {
		//TODO to dziwny warunek
		toDefault();
	}
}

void Controls::hudAction(HudData* hud) {
	state = BUILD;
	typeToCreate = ObjectType::BUILDING;
	idToCreate = hud->getId();
	tempBuildingNode->SetEnabled(false);
}

void Controls::order(short id, ActionParameter& parameter) {
	switch (selectedType) {
	case ObjectType::PHYSICAL:
		orderPhysical(id, parameter);
		break;
	case ObjectType::UNIT:
		actionUnit(id, parameter);
		break;
	case ObjectType::BUILDING:
		orderBuilding(id, parameter);
		break;
	case ObjectType::RESOURCE: break;
	default: ;
	}
}

void Controls::executeOnAll(short id, ActionParameter& parameter) {
	for (auto& phy : *selected) {
		phy->action(id, parameter);
	}
}

void Controls::orderBuilding(short id, ActionParameter& parameter) {
	executeOnAll(id, parameter);
}

void Controls::orderPhysical(short id, ActionParameter& parameter) {
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

void Controls::showNode(Urho3D::Node* node, const hit_data& hitData) const {
	node->SetEnabled(true);
	node->SetScale(1);
	node->SetPosition(hitData.position);
}

bool Controls::clickDown(MouseButton& var, hit_data& hitData) {
	if (raycast(hitData)) {
		var.setFirst(hitData.position);
		return true;
	}
	return false;
}

void Controls::clickDown(MouseButton& var, Urho3D::Node* node) {
	hit_data hitData;
	if (clickDown(var, hitData)) {
		showNode(node, hitData);
	}
}

void Controls::createBuilding(Urho3D::Vector2 pos) {
	if (idToCreate >= 0) {
		auto player = Game::getPlayersManager()->getActivePlayer();

		Game::getCreationCommandList()->addBuilding(idToCreate, pos,
		                                            player->getId(),
		                                            player->getLevelForBuilding(idToCreate)
		                                           );
	}
}

SelectedInfo* Controls::getInfo() {
	return selectedInfo;
}

ControlsState Controls::getState() {
	return state;
}

void Controls::cleanMouse() {
	left.clean();
	right.clean();
	selectionNode->SetEnabled(false);
	arrowNode->SetEnabled(false);
}

void Controls::deactivate() {
	if (active) {
		active = true;
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
		state = ORDER;
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

void Controls::actionUnit(short id, ActionParameter& parameter) {
	switch (parameter.type) {
	case MenuAction::ORDER:
		unitOrder(id);
		break;
	case MenuAction::FORMATION:
		unitFormation(id);
		break;
	default: ;
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

void Controls::clean(SimulationInfo* simulationInfo) {
	bool condition;
	switch (selectedType) {

	case ObjectType::UNIT:
		condition = simulationInfo->ifUnitDied();
		break;
	case ObjectType::BUILDING:
		condition = simulationInfo->ifBuildingDied();
		break;
	case ObjectType::RESOURCE:
		condition = simulationInfo->ifResourceDied();
		break;
	default:
		condition = false;
	}

	if (condition) {
		refreshSelected();
	}
	resetState();
}

void Controls::updateSelection() {
	hit_data hitData;

	if (raycast(hitData, ObjectType::PHYSICAL)) {
		const float xScale = left.held.first->x_ - hitData.position.x_;
		const float zScale = left.held.first->z_ - hitData.position.z_;

		selectionNode->SetScale(Urho3D::Vector3(abs(xScale), 0.1, abs(zScale)));
		selectionNode->SetPosition((*left.held.first + hitData.position) / 2);
	}
}

void Controls::updateArrow() {
	hit_data hitData;

	if (raycast(hitData, ObjectType::PHYSICAL)) {
		auto dir = *right.held.first - hitData.position;

		const float length = dir.Length();
		arrowNode->SetScale(Urho3D::Vector3(length, 1, length / 3));
		arrowNode->SetDirection(Urho3D::Vector3(-dir.z_, 0, dir.x_));
		arrowNode->SetPosition(*right.held.first);
	}
}

void Controls::toDefault() {
	state = DEFAULT;
	cleanMouse();
	idToCreate = -1;
	tempBuildingNode->SetEnabled(false);
}

void Controls::defaultControl() {
	if (input->GetMouseButtonDown(Urho3D::MOUSEB_LEFT)) {
		if (!left.isHeld) {
			clickDown(left, selectionNode);
		} else {
			updateSelection();
		}
	} else if (left.isHeld) {
		releaseLeft();
	}

	if (input->GetMouseButtonDown(Urho3D::MOUSEB_RIGHT)) {
		if (!right.isHeld) {
			clickDown(right, arrowNode);
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
			//TODO OPTM nie robic tego co klatkê
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
			if (orderAction(false)) {
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

void Controls::control() {
	switch (state) {
	case DEFAULT:
		defaultControl();
		break;
	case BUILD:
		buildControl();
		break;
	case ORDER:
		orderControl();
		break;
	}
}

void Controls::unitFormation(short id) {
	Game::getFormationManager()->createFormation(selected, FormationType(id));
}
