#include "Controls.h"
#include "Game.h"
#include "HitData.h"
#include "camera/CameraManager.h"
#include "commands/CommandList.h"
#include "commands/action/ActionCommand.h"
#include "commands/creation/CreationCommandList.h"
#include "database/DatabaseCache.h"
#include "player/PlayersManager.h"
#include "simulation/env/Enviroment.h"
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <algorithm>
#include <queue>


Controls::Controls(Input* _input) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	input = _input;
	selectedInfo = new SelectedInfo();

	createNode("Models/box.mdl", "Materials/green_alpha.xml", &selectionNode);
	createNode("Models/arrow2.mdl", "Materials/dark_red_alpha.xml", &arrowNode);
	selectedType = ObjectType::PHISICAL;
}


Controls::~Controls() {
	delete selectedInfo;
	delete selected;
	selectionNode->Remove();
	arrowNode->Remove();
}

void Controls::createNode(String model, String texture, Node** node) {
	*node = Game::get()->getScene()->CreateChild();
	StaticModel* selectionModel = (*node)->CreateComponent<StaticModel>();
	selectionModel->SetModel(Game::get()->getCache()->GetResource<Model>(model));
	selectionModel->SetMaterial(Game::get()->getCache()->GetResource<Material>(texture));
	(*node)->SetEnabled(false);
}

bool Controls::raycast(hit_data& hitData, Camera* camera) {
	const IntVector2 pos = Game::get()->getUI()->GetCursorPosition();
	if (!Game::get()->getUI()->GetCursor()->IsVisible() || Game::get()->getUI()->GetElementAt(pos, true)) {
		return false;
	}

	const Ray cameraRay = camera->GetScreenRay((float)pos.x_ / Game::get()->getGraphics()->GetWidth(),
	                                           (float)pos.y_ / Game::get()->getGraphics()->GetHeight());

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	Game::get()->getScene()->GetComponent<Octree>()->Raycast(query);
	int i = 0;
	while (i < results.Size()) {
		RayQueryResult& result = results[i];

		Node* node = result.node_;

		LinkComponent* lc = node->GetComponent<LinkComponent>();
		if (lc == nullptr) {
			Node* parent = node->GetParent();
			lc = parent->GetComponent<LinkComponent>();
			if (lc == nullptr) {
				++i;
			} else {
				hitData.position = result.position_;
				hitData.drawable = result.drawable_;
				hitData.link = lc;
				return true;
			}
		} else {
			hitData.position = result.position_;
			hitData.drawable = result.drawable_;
			hitData.link = lc;
			return true;
		}
	}
	hitData.drawable = nullptr;
	hitData.link = nullptr;

	return false;
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
	ObjectType entityType = entity->getType();
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

void Controls::leftClick(Physical* clicked, Vector3& hitPos) {
	if (!input->GetKeyDown(KEY_CTRL)) {
		unSelectAll();
	}
	select(clicked);
}

void Controls::leftClickBuild(Physical* clicked, Vector3& hitPos) {
	unSelectAll();
	createBuilding(Vector2(hitPos.x_, hitPos.z_));
}

void Controls::rightClickDefault(Physical* clicked, Vector3& hitPos, bool shiftPressed) {
	switch (clicked->getType()) {
	case ObjectType::PHISICAL:
		{
		OrderType type;
		if (shiftPressed) {
			type = OrderType::PATROL;
		} else {
			type = OrderType::GO;
		}
		Vector2* newPos = new Vector2(
		                              hitPos.x_,
		                              hitPos.z_
		                             );
		Game::get()->getActionCommandList()->add(new ActionCommand(selected, type, newPos, shiftPressed));
		break;
		}
	case ObjectType::UNIT:
		{
		Game::get()->getActionCommandList()->add(new ActionCommand(selected, OrderType::FOLLOW, clicked, shiftPressed));
		//unSelectAll();
		break;
		}
	case ObjectType::BUILDING:
		Game::get()->getActionCommandList()->add(new ActionCommand(selected, OrderType::FOLLOW, clicked, shiftPressed));
		break;
	case ObjectType::RESOURCE: break;
	default: ;
	}
}

void Controls::leftHold(std::pair<Vector3*, Vector3*>& held) {
	if (!input->GetKeyDown(KEY_CTRL)) {
		unSelectAll();
	}
	std::vector<Physical*>* entities = Game::get()->getEnviroment()->getNeighbours(held);
	for (auto entity : *entities) {
		select(entity); //TODO zastapic wrzuceniem na raz
	}
}

void Controls::rightHold(std::pair<Vector3*, Vector3*>& held) {
	OrderType type[2];

	if (input->GetKeyDown(KEY_SHIFT)) {
		type[0] = OrderType::PATROL;
		type[1] = OrderType::PATROL;
	} else {
		type[0] = OrderType::GO;
		type[1] = OrderType::CHARGE;
	}
	Vector2* newPos = new Vector2(
	                              held.first->x_,
	                              held.first->z_
	                             );
	Vector2* charge = new Vector2(
	                              held.second->x_ - held.first->x_,
	                              held.second->z_ - held.first->z_
	                             );

	Game::get()->getActionCommandList()->add(new ActionCommand(selected, type[0], newPos));
	Game::get()->getActionCommandList()->add(new ActionCommand(selected, type[1], charge));
	//TODO czy ta para jest usuwana
}


void Controls::releaseLeft() {
	hit_data hitData;

	if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
		left.setSecond(hitData.position);
		const float dist = (*left.held.first - *left.held.second).LengthSquared();
		if (dist > clickDistance) {
			leftHold(left.held);
		} else {
			LinkComponent* lc = hitData.link;
			if (lc) {
				leftClick(lc->getPhysical(), hitData.position);
			}
		}
		selectionNode->SetEnabled(false);
		left.clean();
	}
}

void Controls::releaseBuildLeft() {
	hit_data hitData;

	if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
		LinkComponent* lc = hitData.link;
		if (lc) {
			leftClickBuild(lc->getPhysical(), hitData.position);
		}
	}
}

void Controls::releaseRight() {
	hit_data hitData;

	if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
		right.setSecond(hitData.position);
		float dist = (*right.held.first - *right.held.second).LengthSquared();
		if (dist > clickDistance) {
			rightHold(right.held);
		} else {
			LinkComponent* lc = hitData.link;
			if (lc) {
				rightClickDefault(lc->getPhysical(), hitData.position, input->GetKeyDown(KEY_SHIFT));
			}
		}
		arrowNode->SetEnabled(false);
		right.clean();
	}
}

bool Controls::orderAction(bool shiftPressed) {
	hit_data hitData;

	if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
		LinkComponent* lc = hitData.link;
		if (lc) {
			rightClickDefault(lc->getPhysical(), hitData.position, shiftPressed);
			return true;
		}
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
}

void Controls::order(short id, ActionParameter& parameter) {
	switch (selectedType) {
	case ObjectType::PHISICAL:
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

void Controls::orderBuilding(short id, ActionParameter& parameter) {
	for (auto& phy : *selected) {
		phy->action(id, parameter);
	}
}

void Controls::orderPhysical(short id, ActionParameter& parameter) {
	int level = Game::get()->getPlayersManager()->getActivePlayer()->getLevelForBuilding(id) + 1;

	Resources& resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();

	switch (parameter.type) {
	case ActionType::BUILDING_LEVEL:
		{
		optional<std::vector<db_cost*>*> opt = Game::get()->getDatabaseCache()->getCostForBuildingLevel(id, level);
		if (opt.has_value()) {
			auto costs = opt.value();
			if (resources.reduce(costs)) {
				Game::get()->getQueueManager()->add(1, parameter.type, id, 1);
			}
		}
		break;
		}
	}

}

void Controls::startSelectionNode(hit_data hitData) {
	selectionNode->SetEnabled(true);
	selectionNode->SetScale(1);
	int y = selectionNode->GetPosition().y_;
	Vector3 newPos = hitData.position;
	newPos.y_ = y;
	selectionNode->SetPosition(newPos);
}

void Controls::startArrowNode(const hit_data& hitData) {
	arrowNode->SetEnabled(true);
	arrowNode->SetScale(1);
	int y = arrowNode->GetPosition().y_;
	Vector3 newPos = hitData.position;
	newPos.y_ = y;
	arrowNode->SetPosition(newPos);
}

bool Controls::clickDown(MouseButton& var, hit_data hitData) {
	bool clicked = raycast(hitData, Game::get()->getCameraManager()->getComponent());
	if (clicked) {
		var.setFirst(hitData.position);
	}
	return clicked;
}

void Controls::clickDownLeft() {
	hit_data hitData;
	bool clicked = clickDown(left, hitData);
	if (clicked) {
		startSelectionNode(hitData);
	}
}

void Controls::clickDownRight() {
	hit_data hitData;
	bool clicked = clickDown(right, hitData);
	if (clicked) {
		startArrowNode(hitData);
	}
}

void Controls::createBuilding(Vector2& pos) {
	if (idToCreate >= 0) {
		Game::get()->getCreationCommandList()->addBuilding(
		                                                   idToCreate, pos,
		                                                   Game::get()->getPlayersManager()->getActivePlayer()->getId(),
		                                                   Game::get()->getPlayersManager()->getActivePlayer()->
		                                                                getLevelForBuilding(idToCreate)
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
	OrderType type = OrderType(id);
	switch (type) {
	case OrderType::GO:
	case OrderType::CHARGE:
	case OrderType::ATTACK:
	case OrderType::PATROL:
	case OrderType::FOLLOW:
		state = ORDER;
		orderType = type;
		break;
	case OrderType::STOP:
	case OrderType::DEFEND:
	case OrderType::DEAD:
		for (auto& phy : *selected) {
			ActionParameter empty;
			phy->action(id, empty); //TODO przemyslec to
		}
		break;
	default: ;
	}
}

void Controls::actionUnit(short id, ActionParameter& parameter) {
	switch (parameter.type) {
	case ActionType::ORDER:
		unitOrder(id);
		break;
	case ActionType::FORMATION:
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
		                               } else {
			                               return false;
		                               }
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

	if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
		float xScale = left.held.first->x_ - hitData.position.x_;
		float zScale = left.held.first->z_ - hitData.position.z_;

		Vector3 center = (*left.held.first + hitData.position) / 2;
		selectionNode->SetScale(Vector3(abs(xScale), 0.5, abs(zScale)));
		center.y_ += 1;
		selectionNode->SetPosition(center);
	}
}

void Controls::updateArrow() {
	hit_data hitData;

	if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
		Vector3 dir = *right.held.first - hitData.position;

		Vector3 pos = *right.held.first;
		float length = dir.Length();
		arrowNode->SetScale(Vector3(length, 1, length / 3));
		arrowNode->SetDirection(Vector3(-dir.z_, 0, dir.x_));
		pos.y_ += 1;
		arrowNode->SetPosition(pos);
	}
}

void Controls::toDefault() {
	state = DEFAULT;
	cleanMouse();
	idToCreate = -1;
}

void Controls::defaultControl() {
	if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
		if (!left.isHeld) {
			clickDownLeft();
		} else {
			updateSelection();
		}
	} else if (left.isHeld) {
		releaseLeft();
	}

	if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
		if (!right.isHeld) {
			clickDownRight();
		} else {
			updateArrow();
		}
	} else if (right.isHeld) {
		releaseRight();
	}
	if (input->GetMouseButtonDown(MOUSEB_MIDDLE)) {
		hit_data hitData;

		if (raycast(hitData, Game::get()->getCameraManager()->getComponent())) {
			selectedInfo->setMessage(hitData.position.ToString());
		}
	}
}

void Controls::buildControl() {
	if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
		if (!left.isHeld) {
			left.markHeld();
		}
	} else if (left.isHeld) {
		releaseBuildLeft();
	}

	if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
		toDefault();
	}
}

void Controls::orderControl() {
	if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
		if (!left.isHeld) {
			left.markHeld();
		}
	} else if (left.isHeld) {
		switch (orderType) {
		case OrderType::GO:
		case OrderType::ATTACK:
		case OrderType::FOLLOW:
			if (orderAction(false)) {
				toDefault();
			}
			break;
		case OrderType::PATROL:
			orderAction(true);
			break;
		case OrderType::CHARGE:

			break;
		default: ;
		}
		left.clean();
	}

	if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
		if (!right.isHeld) {
			right.markHeld();
		}
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
	FormationType type = FormationType(id);
	Game::get()->getFormationManager()->createFormation(selected, type);
}
