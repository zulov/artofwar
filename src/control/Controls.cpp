#include "Controls.h"
#include "commands/ActionCommand.h"
#include "commands/CommandList.h"
#include "commands/SimulationCommandList.h"
#include <algorithm>

Controls::Controls(Input* _input) {
	selected = new std::vector<Physical*>();
	selected->reserve(5000);

	leftHeld = new std::pair<Vector3*, Vector3*>();
	middleHeld = new std::pair<Vector3*, Vector3*>();
	rightHeld = new std::pair<Vector3*, Vector3*>();
	input = _input;
	selectedInfo = new SelectedInfo();
}


Controls::~Controls() {
	delete leftHeld;
	delete middleHeld;
	delete rightHeld;
	delete selectedInfo;
	delete selected;
}

bool Controls::raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera) {
	hitDrawable = nullptr;

	IntVector2 pos = Game::get()->getUI()->GetCursorPosition();
	if (!Game::get()->getUI()->GetCursor()->IsVisible() || Game::get()->getUI()->GetElementAt(pos, true)) {
		return false;
	}

	Ray cameraRay = camera->GetScreenRay((float)pos.x_ / Game::get()->getGraphics()->GetWidth(), (float)pos.y_ / Game::get()->getGraphics()->GetHeight());

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

void Controls::controlEntity(Vector3& hitPos, bool ctrlPressed, Physical* clicked) {
	switch (state) {
	case DEFAULT:
		{
		if (!ctrlPressed) {
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

void Controls::leftClick(Drawable* hitDrawable, Vector3& hitPos) {
	Node* hitNode = hitDrawable->GetNode();
	bool ctrlPressed = input->GetKeyDown(KEY_CTRL);
	LinkComponent* lc = hitNode->GetComponent<LinkComponent>();

	if (lc == nullptr) {
		return;
	}
	Physical* clicked = lc->getPhysical();
	ObjectType type = clicked->getType();

	switch (type) {
	case PHISICAL:
		controlEntity(hitPos, ctrlPressed, clicked);
		break;
	case UNIT:
	case BUILDING:
	case RESOURCE:
		if (!ctrlPressed) {
			unSelectAll();
		}

		select(clicked);
		break;

	default: ;
	}

}

void Controls::rightClick(Drawable* hitDrawable, Vector3& hitPos) {
	Node* hitNode = hitDrawable->GetNode();
	bool shiftPressed = input->GetKeyDown(KEY_SHIFT);
	LinkComponent* lc = hitNode->GetComponent<LinkComponent>();
	if (lc == nullptr) {
		return;
	}
	Physical* clicked = lc->getPhysical();
	ObjectType type = clicked->getType();

	switch (type) {

	case PHISICAL:
		{
		Vector3* pos = new Vector3(hitPos);
		ActionCommand* command;
		if (shiftPressed) {
			command = new ActionCommand(selected, OrderType::PATROL, pos);
		} else {
			command = new ActionCommand(selected, OrderType::GO, pos);
		}

		Game::get()->getActionCommandList()->add(command);
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
	std::vector<Physical*>* entities = Game::get()->getMediator()->getEntities(held);
	bool ctrlPressed = input->GetKeyDown(KEY_CTRL);
	if (!ctrlPressed) {
		unSelectAll();
	}
	for (int i = 0; i < entities->size(); ++i) {//TODO zastapic wrzuceniem na raz
		select((*entities)[i]);
	}
	delete entities;
}

void Controls::rightHold(std::pair<Vector3*, Vector3*>* pair) {
	Vector3* pos1 = new Vector3(*pair->first);
	Vector3* pos2 = new Vector3(*pair->second);//TODO czy ta para jest usuwana
	ActionCommand* command1;
	ActionCommand* command2;

	bool shiftPressed = input->GetKeyDown(KEY_SHIFT);

	if (shiftPressed) {
		command1 = new ActionCommand(selected, OrderType::PATROL, pos1);
		command2 = new ActionCommand(selected, OrderType::PATROL, pos2);
	} else {
		command1 = new ActionCommand(selected, OrderType::GO, pos1);
		command2 = new ActionCommand(selected, OrderType::PATROL, pos2);
	}

	Game::get()->getActionCommandList()->add(command1);
	Game::get()->getActionCommandList()->add(command2);
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
					leftClick(hitDrawable, hitPos);
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
					rightClick(hitDrawable, hitPos);
				}
			}
		}
		break;
	case MOUSEB_MIDDLE:
		if (mouseMiddleHeld == true) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				mouseMiddleHeld = false;
				middleHeld->second = new Vector3(hitPos);
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

void Controls::clickDownRight(Vector3& hitPos) {
	if (rightHeld->first != nullptr) {
		delete rightHeld->first;
		rightHeld->first = nullptr;
	}
	rightHeld->first = new Vector3(hitPos);
}

void Controls::clickDownLeft(Vector3& hitPos) {
	if (leftHeld->first != nullptr) {
		delete leftHeld->first;
		leftHeld->first = nullptr;
	}
	leftHeld->first = new Vector3(hitPos);
}

void Controls::clickDown(const int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	switch (button) {
	case MOUSEB_LEFT:
		if (mouseLeftHeld == false) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				clickDownLeft(hitPos);
				mouseLeftHeld = true;
			}
		}
		break;
	case MOUSEB_RIGHT:
		if (mouseRightHeld == false) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				clickDownRight(hitPos);
				mouseRightHeld = true;
			}
		}
		break;
	case MOUSEB_MIDDLE:
		if (mouseMiddleHeld == false) {
			if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
				mouseMiddleHeld = true;
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
			SimulationCommand* simulationCommand = new SimulationCommand(type, number, idToCreate, pos, SpacingType::CONSTANT, 0);
			Game::get()->getSimCommandList()->add(simulationCommand);
		}
	}
}

SelectedInfo* Controls::getInfo() {
	return selectedInfo;
}

void Controls::deactivate() {
	active = false;
	mouseMiddleHeld = false;
	mouseRightHeld = false;
	mouseLeftHeld = false;
	if (rightHeld->first != nullptr) {
		delete rightHeld->first;
		rightHeld->first = nullptr;
	}
	if (rightHeld->second != nullptr) {
		delete rightHeld->second;
		rightHeld->second = nullptr;
	}

	if (leftHeld->first != nullptr) {
		delete leftHeld->first;
		leftHeld->first = nullptr;
	}
	if (leftHeld->second != nullptr) {
		delete leftHeld->second;
		leftHeld->second = nullptr;
	}

	if (middleHeld->first != nullptr) {
		delete middleHeld->first;
		middleHeld->first = nullptr;
	}
	if (middleHeld->second != nullptr) {
		delete middleHeld->second;
		middleHeld->second = nullptr;
	}
}

bool Controls::isActive() {
	return active;
}

void Controls::activate() {
	active = true;
}

void Controls::action(HudElement* hudElement) {
	short id = hudElement->getId();
	for (int i = 0; i < selected->size(); ++i) {
		(*selected)[i]->action(id,nullptr);//TODO przemyslec to
	}
}

void Controls::refreshSelected() {
	int preSize = selected->size();
	selected->erase(
	                std::remove_if(
	                               selected->begin(), selected->end(),
	                               [](Physical* physical) {
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
