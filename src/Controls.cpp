#include "Controls.h"
#include "ActionCommand.h"
#include "CommandList.h"
#include "SimulationCommandList.h"

Controls::Controls(Input* _input) {
	selected = new std::vector<Physical*>();
	selected->reserve(100);

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

bool Controls::raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera) {//TODO nie robic tak czesto
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

void Controls::unSelect(int type) {
	for (int i = 0; i < selected->size(); i++) {
		(*selected)[i]->unSelect();
	}
	selected->clear();
	selectedType = ObjectType(type);
	selectedInfo->setSelectedType(selectedType);
	selectedInfo->reset();
}

//void Controls::action(ActionType action, Entity* entity) {
//	for (int i = 0; i < selected->size(); i++) {
//		selected->at(i)->action(action, entity);
//	}
//}

void Controls::select(Physical* entity) {
	ObjectType entityType = entity->getType();
	if (entity->getType() != selectedType) {
		unSelect(entity->getType());
	}

	entity->select();
	selected->push_back(entity);
	int subType = entity->getSubType();
	selectedInfo->incNumberAt(subType);
	selectedType = entityType;
	selectedInfo->setSelectedType(selectedType);
	selectedInfo->setAllNumber(selected->size());
}

void Controls::controlEntity(Vector3 hitPos, bool ctrlPressed, Physical* clicked) {
	switch (state) {
	case SELECT:
		{
		if (!ctrlPressed) {
			unSelect(ENTITY);
		}
		select(clicked);
		}
		break;
	case BUILD:
		{
		unSelect(ENTITY);
		build(new Vector3(hitPos));
		break;
		}
	case DEPLOY:
		{
		unSelect(ENTITY);
		deploy(new Vector3(hitPos));
		break;
		}

	}
}

void Controls::leftClick(Drawable* hitDrawable, Vector3 hitPos) {//TODO referencja
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
		if (!ctrlPressed) {
			unSelect(ENTITY);
		}

		select(clicked);
		break;

	case BUILDING:
		if (!ctrlPressed) {
			unSelect(ENTITY);
		}

		select(clicked);
		break;

	case RESOURCE:
		if (!ctrlPressed) {
			unSelect(ENTITY);
		}
		select(clicked);
		break;

	default: ;
	}

}

void Controls::rightClick(Drawable* hitDrawable, Vector3 hitPos) {
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
			command = new ActionCommand(selected, APPEND_AIM, pos);
		} else {
			command = new ActionCommand(selected, ADD_AIM, pos);
		}

		Game::get()->getActionCommandList()->add(command);
		break;
		}
	case UNIT:
		{
		unSelect(ENTITY);
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
		unSelect(ENTITY);
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
		command1 = new ActionCommand(selected, APPEND_AIM, pos1);
		command2 = new ActionCommand(selected, APPEND_AIM, pos2);
	} else {
		command1 = new ActionCommand(selected, ADD_AIM, pos1);
		command2 = new ActionCommand(selected, APPEND_AIM, pos2);
	}

	Game::get()->getActionCommandList()->add(command1);
	Game::get()->getActionCommandList()->add(command2);
}

void Controls::release(const int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;
	if (mouseLeftHeld == true || mouseRightHeld == true || mouseMiddleHeld == true) {
		if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
			switch (button) {
			case MOUSEB_LEFT:
				if (mouseLeftHeld == true) {
					mouseLeftHeld = false;
					leftHeld->second = new Vector3(hitPos);
					double dist = (*(leftHeld->first) - *(leftHeld->second)).LengthSquared();
					if (dist > clickDistance) {
						leftHold(leftHeld);
					} else {
						leftClick(hitDrawable, hitPos);
					}
				}
				break;
			case MOUSEB_RIGHT:
				if (mouseRightHeld == true) {
					mouseRightHeld = false;
					rightHeld->second = new Vector3(hitPos);//TODO czy to entity jest usywane?
					double dist = (*(rightHeld->first) - *(rightHeld->second)).LengthSquared();
					if (dist > clickDistance) {
						rightHold(rightHeld);
					} else {
						rightClick(hitDrawable, hitPos);
					}
				}
				break;
			case MOUSEB_MIDDLE:
				if (mouseMiddleHeld == true) {
					mouseMiddleHeld = false;
					middleHeld->second = new Vector3(hitPos);
				}
				break;
			}
		}
	}
}

void Controls::updateState(ControlsState state) {
	this->state = state;
}

void Controls::hudAction(HudElement* hud) {
	if (hud->isBuildType1()) {
		BuildingType type = hud->getBuildingType();
		if (state == ControlsState::BUILD) {
			toBuild = type;
		}
	}
	if (hud->isUnitType1()) {
		UnitType type = hud->getUnitType();
		if (state == ControlsState::DEPLOY) {
			toDeploy = type;
		}
	}
}

void Controls::clickDownRight(Vector3 hitPos) {
	if (rightHeld->first != nullptr) {
		delete rightHeld->first;
		rightHeld->first = nullptr;
	}
	rightHeld->first = new Vector3(hitPos);
}

void Controls::clickDownLeft(Vector3 hitPos) {
	if (leftHeld->first != nullptr) {
		delete leftHeld->first;
		leftHeld->first = nullptr;
	}
	leftHeld->first = new Vector3(hitPos);
}

void Controls::clickDown(const int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
		//Node* hitNode = hitDrawable->GetNode();
		switch (button) {
		case MOUSEB_LEFT:
			if (mouseLeftHeld == false) {
				clickDownLeft(hitPos);
				mouseLeftHeld = true;
			}
			break;
		case MOUSEB_RIGHT:
			if (mouseRightHeld == false) {
				clickDownRight(hitPos);
				mouseRightHeld = true;
			}
			break;
		case MOUSEB_MIDDLE:
			if (mouseMiddleHeld == false) {
				mouseMiddleHeld = true;
			}
			break;
		}
	}
}

void Controls::build(Vector3* pos) {
	SimulationCommand* simulationCommand = new SimulationCommand(1, toBuild, pos, SpacingType::CONSTANT, 0);//TODO pobrac parametry z jakiegos zrod³a
	Game::get()->getSimCommandList()->add(simulationCommand);
}

void Controls::deploy(Vector3* pos) {
	SimulationCommand* simulationCommand = new SimulationCommand(10, toDeploy, pos, SpacingType::CONSTANT, 0);
	Game::get()->getSimCommandList()->add(simulationCommand);
}

SelectedInfo* Controls::getSelectedInfo() {
	return selectedInfo;
}
