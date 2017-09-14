#include "Controls.h"
#include "commands/ActionCommand.h"
#include "commands/CommandList.h"
#include "commands/SimulationCommandList.h"
#include <algorithm>

Controls::Controls(Input* _input) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	leftHeld = new std::pair<Vector3*, Vector3*>();
	rightHeld = new std::pair<Vector3*, Vector3*>();
	input = _input;
	selectedInfo = new SelectedInfo();
}


Controls::~Controls() {
	cleanPair(rightHeld);
	cleanPair(leftHeld);

	delete leftHeld;
	delete rightHeld;

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
	if (entity->getType() != selectedType) {
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

void Controls::rightClick(Physical* clicked, Vector3& hitPos) {
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
		unSelectAll();
		break;
		}
	case BUILDING: break;
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
		type[1] = OrderType::PATROL;
	}

	Game::get()->getActionCommandList()->add(new ActionCommand(selected, type[0], new Vector3(*held->first)));
	Game::get()->getActionCommandList()->add(new ActionCommand(selected, type[1], new Vector3(*held->second)));//TODO czy ta para jest usuwana
}

void Controls::release(const int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	switch (button) {
	case MOUSEB_LEFT:
		if (mouseLeftHeld == true) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				mouseLeftHeld = false;
				leftHeld->second = new Vector3(hitPos);
				double dist = (*(leftHeld->first) - *(leftHeld->second)).LengthSquared();
				if (dist > clickDistance) {
					leftHold(leftHeld);
				} else {
					Node* hitNode = hitDrawable->GetNode();
					LinkComponent* lc = hitNode->GetComponent<LinkComponent>();

					if (lc) {
						Physical* clicked = lc->getPhysical();
						leftClick(clicked, hitPos);
					}
				}
			}
		}
		break;
	case MOUSEB_RIGHT:
		if (mouseRightHeld == true) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				mouseRightHeld = false;
				rightHeld->second = new Vector3(hitPos);//TODO czy ten Vector jest usuwany?
				double dist = (*(rightHeld->first) - *(rightHeld->second)).LengthSquared();
				if (dist > clickDistance) {
					rightHold(rightHeld);
				} else {
					Node* hitNode = hitDrawable->GetNode();
					LinkComponent* lc = hitNode->GetComponent<LinkComponent>();
					if (lc) {
						Physical* clicked = lc->getPhysical();
						rightClick(clicked, hitPos);
					}

				}
			}
		}
		break;
	}
}

void Controls::updateState(SelectedInfo* selectedInfo) {
	state = DEFAULT;
	idToCreate = -1;
}

void Controls::hudAction(HudElement* hud) {
	typeToCreate = hud->getType();
	state = BUILD;
	idToCreate = hud->getId();
}

void Controls::setFirst(Vector3& hitPos, std::pair<Vector3*, Vector3*>* var) {
	if (var->first != nullptr) {
		delete var->first;
		var->first = nullptr;
	}
	var->first = new Vector3(hitPos);
}

void Controls::clickDown(const int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	switch (button) {
	case MOUSEB_LEFT:
		if (mouseLeftHeld == false) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				setFirst(hitPos, leftHeld);
				mouseLeftHeld = true;
			}
		}
		break;
	case MOUSEB_RIGHT:
		if (mouseRightHeld == false) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				setFirst(hitPos, rightHeld);
				mouseRightHeld = true;
			}
		}
		break;
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

void Controls::cleanPair(std::pair<Vector3*, Vector3*>* var) {
	if (var->first != nullptr) {
		delete var->first;
		var->first = nullptr;
	}
	if (var->second != nullptr) {
		delete var->second;
		var->second = nullptr;
	}
}

void Controls::deactivate() {
	active = false;

	mouseRightHeld = false;
	mouseLeftHeld = false;

	cleanPair(rightHeld);
	cleanPair(leftHeld);
}

void Controls::activate() {
	active = true;
}

void Controls::order(short id) {
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
}

void Controls::control() {
	if (active) {
		if (input->GetMouseButtonDown(MOUSEB_LEFT)) {
			clickDown(MOUSEB_LEFT);
		} else {
			release(MOUSEB_LEFT);
		}

		if (input->GetMouseButtonDown(MOUSEB_RIGHT)) {
			clickDown(MOUSEB_RIGHT);
		} else {
			release(MOUSEB_RIGHT);
		}
	}
}
