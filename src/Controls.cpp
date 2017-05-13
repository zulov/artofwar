#include "Controls.h"
#include "ActionCommand.h"
#include "CommandList.h"
#include "SimulationCommandList.h"

Controls::Controls(Input* _input) {
	selected = new std::vector<Entity*>();
	selected->reserve(25);

	leftHeld = new std::pair<Entity*, Entity*>();
	middleHeld = new std::pair<Entity*, Entity*>();
	rightHeld = new std::pair<Entity*, Entity*>();
	input = _input;
}


Controls::~Controls() {
	delete leftHeld;
	delete middleHeld;
	delete rightHeld;
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
	Game::get()->getScene()->GetComponent<Octree>()->RaycastSingle(query);
	if (results.Size()) {
		RayQueryResult& result = results[0];
		hitPos = result.position_;
		hitDrawable = result.drawable_;
		return true;
	}

	return false;
}

void Controls::unSelect(int type) {
	selectedType = ObjectType(type);
	for (int i = 0; i < selected->size(); i++) {
		(*selected)[i]->unSelect();
	}
	selected->clear();
}

//void Controls::action(ActionType action, Entity* entity) {
//	for (int i = 0; i < selected->size(); i++) {
//		selected->at(i)->action(action, entity);
//	}
//}

void Controls::select(Entity* entity) {
	if (entity->getType() == selectedType) {

	} else {
		unSelect(entity->getType());
	}
	entity->select();
	selected->push_back(entity);
}

void Controls::leftClick(Drawable* hitDrawable, Vector3 hitPos) {
	Node* hitNode = hitDrawable->GetNode();
	bool ctrlPressed = input->GetKeyDown(KEY_CTRL);

	if (hitNode->GetName() == "Box") {
		if (!ctrlPressed) {
			unSelect(ENTITY);
		}
		LinkComponent* lc = hitNode->GetComponent<LinkComponent>();
		Entity* clicked = lc->getEntity();
		select(clicked);
	} else if (hitNode->GetName() == "Ground") {
		switch (controlsState) {
		case SELECT: {
			if (!ctrlPressed) {
				unSelect(ENTITY);
			}
			LinkComponent* lc = hitNode->GetComponent<LinkComponent>();
			Entity* clicked = lc->getEntity();
			select(clicked);
		}
			break;
		case BUILD: {
			unSelect(ENTITY);
			build(new Vector3(hitPos));
		}
			break;
		}
	}
}

void Controls::rightClick(Drawable* hitDrawable, Vector3 hitPos) {
	Node* hitNode = hitDrawable->GetNode();
	bool shiftPressed = input->GetKeyDown(KEY_SHIFT);
	if (hitNode->GetName() == "Box") {
		unSelect(ENTITY);
	} else if (hitNode->GetName() == "Ground") {
		Vector3* pos = new Vector3(hitPos);
		ActionCommand* command;
		if (shiftPressed) {
			command = new ActionCommand(selected, APPEND_AIM, pos);
		} else {
			command = new ActionCommand(selected, ADD_AIM, pos);
		}

		Game::get()->getActionCommandList()->add(command);
	}
}

void Controls::leftHold(std::pair<Entity*, Entity*>* held) {
	std::vector<Entity*>* entities = Game::get()->getMediator()->getEntities(held);
	bool ctrlPressed = input->GetKeyDown(KEY_CTRL);
	if (!ctrlPressed) {
		unSelect(ENTITY);
	}
	for (int i = 0; i < entities->size(); ++i) {//TODO zastapic wrzuceniem na raz
		select((*entities)[i]);
	}
	delete entities;
}

void Controls::rightHold(std::pair<Entity*, Entity*>* pair) {
	Vector3* pos1 = new Vector3(*pair->first->getPosition());
	Vector3* pos2 = new Vector3(*pair->second->getPosition());//TODO czy ta para jest usuwana
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

	if (raycast(hitPos, hitDrawable, Game::get()->getCameraManager()->getComponent())) {
		Entity* entity = new Entity(new Vector3(hitPos), nullptr, nullptr);
		switch (button) {
		case MOUSEB_LEFT:
			if (mouseLeftHeld == true) {
				mouseLeftHeld = false;
				leftHeld->second = entity;
				double dist = (*(leftHeld->first->getPosition()) - *(leftHeld->second->getPosition())).Length();
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
				rightHeld->second = entity;
				double dist = (*(rightHeld->first->getPosition()) - *(rightHeld->second->getPosition())).Length();
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
				middleHeld->second = entity;
			}
			break;
		}
	}
}

void Controls::updateState() {
	if (input->GetKeyDown(KEY_8)) {
		controlsState = SELECT;
	} else if (input->GetKeyDown(KEY_9)) {
		controlsState = BUILD;
	}
}

void Controls::clickDownRight(Vector3 hitPos) {
	Entity* entity = new Entity(new Vector3(hitPos), nullptr, nullptr);

	rightHeld->first = entity;
}

void Controls::clickDownLeft(Vector3 hitPos) {
	Entity* entity = new Entity(new Vector3(hitPos), nullptr, nullptr);

	leftHeld->first = entity;
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
	SimulationCommand* simulationCommand = new SimulationCommand(1, BuildingType::TOWER, pos, SpacingType::CONSTANT);
	Game::get()->getSimCommandList()->add(simulationCommand);
}
