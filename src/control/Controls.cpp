#include "Controls.h"
#include "commands/ActionCommand.h"
#include "commands/CommandList.h"
#include "commands/SimulationCommandList.h"
#include <algorithm>

Controls::Controls(Input* _input) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	input = _input;
	selectedInfo = new SelectedInfo();
}


Controls::~Controls() {
	delete selectedInfo;
	delete selected;
}

bool Controls::raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera) {
	const IntVector2 pos = Game::get()->getUI()->GetCursorPosition();
	if (!Game::get()->getUI()->GetCursor()->IsVisible() || Game::get()->getUI()->GetElementAt(pos, true)) {
		return false;
	}

	Ray cameraRay = camera->GetScreenRay((float)pos.x_ / Game::get()->getGraphics()->GetWidth(),
	                                     (float)pos.y_ / Game::get()->getGraphics()->GetHeight());

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	Game::get()->getScene()->GetComponent<Octree>()->Raycast(query);
	int i = 0;
	while (i < results.Size()) {
		RayQueryResult& result = results[i];

		Node* node = result.node_;
		LinkComponent* lc = node->GetComponent<LinkComponent>();
		if (lc == nullptr) { ++i; } else {
			hitPos = result.position_;
			hitDrawable = result.drawable_;
			return true;
		}
	}
	hitDrawable = nullptr;

	return false;
}

void Controls::unSelectAll() {
	for (int i = 0; i < selected->size(); ++i) {
		(*selected)[i]->unSelect();
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

void Controls::controlEntity(Vector3& hitPos, Physical* clicked) {
	switch (state) {
	case DEFAULT:
		{
		if (!input->GetKeyDown(KEY_CTRL)) {
			unSelectAll();
		}
		select(clicked);
		}
		break;
	case BUILD:
		{
		unSelectAll();
		create(ObjectType::BUILDING, new Vector3(hitPos), 1);
		break;
		}
	}
}

void Controls::leftClick(Physical* clicked, Vector3& hitPos) {
	switch (clicked->getType()) {
	case PHISICAL:
		controlEntity(hitPos, clicked);
		break;
	case UNIT:
	case BUILDING:
	case RESOURCE:
		if (!input->GetKeyDown(KEY_CTRL)) {
			unSelectAll();
		}
		select(clicked);
		break;
	default: ;
	}
}

void Controls::rightClickDefault(Physical* clicked, Vector3& hitPos) {
	switch (clicked->getType()) {
	case PHISICAL:
		{
		OrderType type;
		if (input->GetKeyDown(KEY_SHIFT)) {
			type = OrderType::PATROL;
		} else {
			type = OrderType::GO;
		}
		Game::get()->getActionCommandList()->add(new ActionCommand(selected, type, new Vector3(hitPos)));
		break;
		}
	case UNIT:
		{
		Game::get()->getActionCommandList()->add(new ActionCommand(selected, OrderType::FOLLOW, clicked));
		//unSelectAll();
		break;
		}
	case BUILDING: 
		Game::get()->getActionCommandList()->add(new ActionCommand(selected, OrderType::FOLLOW, clicked));
		break;
	case RESOURCE: break;
	default: ;
	}
}

void Controls::leftHold(std::pair<Vector3*, Vector3*>* held) {
	if (!input->GetKeyDown(KEY_CTRL)) {
		unSelectAll();
	}
	std::vector<Physical*>* entities = Game::get()->getMediator()->getEntities(held);
	for (auto entity : (* entities)) {
		select(entity); //TODO zastapic wrzuceniem na raz
	}
	delete entities;
}

void Controls::rightHold(std::pair<Vector3*, Vector3*>* held) {
	OrderType type[2];

	if (input->GetKeyDown(KEY_SHIFT)) {
		type[0] = OrderType::PATROL;
		type[1] = OrderType::PATROL;
	} else {
		type[0] = OrderType::GO;
		type[1] = OrderType::CHARGE;
	}

	Game::get()->getActionCommandList()->add(new ActionCommand(selected, type[0], new Vector3(*held->first)));
	Game::get()->getActionCommandList()->add(new ActionCommand(selected, type[1],
	                                                           new Vector3(*held->second - *held->first)));
	//TODO czy ta para jest usuwana
}


void Controls::releaseLeft() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
		left.setSecond(hitPos);
		double dist = (*(left.held->first) - *(left.held->second)).LengthSquared();
		if (dist > clickDistance) {
			leftHold(left.held);
		} else {
			LinkComponent* lc = hitDrawable->GetNode()->GetComponent<LinkComponent>();
			if (lc) {
				leftClick(lc->getPhysical(), hitPos);
			}
		}
		left.clean();
	}
}

void Controls::releaseBuildLeft() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
		LinkComponent* lc = hitDrawable->GetNode()->GetComponent<LinkComponent>();
		if (lc) {
			leftClick(lc->getPhysical(), hitPos);
		}
	}
}

void Controls::releaseRight() {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
		right.setSecond(hitPos);
		double dist = (*(right.held->first) - *(right.held->second)).LengthSquared();
		if (dist > clickDistance) {
			rightHold(right.held);
		} else {
			LinkComponent* lc = hitDrawable->GetNode()->GetComponent<LinkComponent>();
			if (lc) {
				rightClickDefault(lc->getPhysical(), hitPos);
			}
		}
		right.clean();
	}
}

void Controls::resetState() {
	if (selectedInfo->hasChanged()) {
		//TODO to dziwny warunek
		state = DEFAULT;
		idToCreate = -1;
	}
}

void Controls::hudAction(HudElement* hud) {
	typeToCreate = hud->getType();
	state = BUILD;
	idToCreate = hud->getId();
}

void Controls::clickDown(MouseButton& var) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
		var.setFirst(hitPos);
	}
}

void Controls::create(ObjectType type, Vector3* pos, int number) {
	if (idToCreate >= 0) {
		Resources* resources = Game::get()->getPlayersManager()->getActivePlayer()->getResources();
		std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(idToCreate);

		if (resources->reduce(costs)) {
			SimulationCommand* simulationCommand = new SimulationCommand
				(type, number, idToCreate, pos, SpacingType::CONSTANT, 0);
			Game::get()->getSimCommandList()->add(simulationCommand);
		}
	}
}

SelectedInfo* Controls::getInfo() {
	return selectedInfo;
}

void Controls::deactivate() {
	active = false;
	left.clean();
	right.clean();
}

void Controls::activate() {
	active = true;
}

void Controls::orderUnit(short id) {
	OrderType type = OrderType(id);
	switch (type) {
	case OrderType::GO:
	case OrderType::CHARGE:
	case OrderType::ATTACK:
	case OrderType::PATROL:
	case OrderType::FOLLOW:
		state = ControlsState::ORDER;
		orderType = type;
		break;
	case OrderType::STOP:
	case OrderType::DEFEND:
	case OrderType::DEAD:
		for (int i = 0; i < selected->size(); ++i) {
			(*selected)[i]->action(id, nullptr);//TODO przemyslec to
		}
		break;
	default: ;
	}
}

void Controls::orderBuilding(short id) {
	for (int i = 0; i < selected->size(); ++i) {
		(*selected)[i]->action(id, nullptr);//TODO przemyslec to
	}
}

void Controls::order(short id) {
	switch (selectedType) {

	case UNIT: 
		orderUnit(id);
		break;
	case BUILDING: 
		orderBuilding(id);
		break;
	case RESOURCE: break;
	default: ;
	}
	
}

void Controls::refreshSelected() {
	int preSize = selected->size();
	selected->erase(
	                std::remove_if(
	                               selected->begin(), selected->end(),
	                               [](Physical* physical)
                               {
	                               if (!physical->isAlive()) {
		                               return true;
	                               }
	                               return false;
                               }),
	                selected->end());
	if (selected->size() != preSize) {
		unSelectAll();
		for (auto physical : (*selected)) {
			select(physical);
		}
	}
}

void Controls::clean(SimulationInfo* simulationInfo) {
	bool condition;
	switch (selectedType) {

	case UNIT:
		condition = simulationInfo->ifUnitDied();
		break;
	case BUILDING:
		condition = simulationInfo->ifBuildingDied();
		break;
	case RESOURCE:
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

void Controls::control() {
	if (active) {

		switch (state) {

		case DEFAULT:
			if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
				if (!left.isHeld) {
					clickDown(left);
				}
			} else if (left.isHeld) {
				releaseLeft();
			}

			if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
				if (!right.isHeld) {
					clickDown(right);
				}
			} else if (right.isHeld) {
				releaseRight();
			}
			break;
		case BUILD:
			if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
				releaseBuildLeft();
			}

			if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
				state = DEFAULT;
				idToCreate = -1;
			}
			break;
		case ORDER: break;
		default: ;
		}

	}
}
