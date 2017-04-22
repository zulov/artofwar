#include "Controls.h"
#include "Game.h"
#include "Main.h"
#include "CommandSelect.h"


Controls::Controls() {
	selected = new std::vector<Entity*>();
	selected->reserve(10);

	leftHeld = new std::pair<Entity*, Entity*>();
	middleHeld = new std::pair<Entity*, Entity*>();
	rightHeld = new std::pair<Entity*, Entity*>();
}


Controls::~Controls() {
	delete leftHeld;
	delete middleHeld;
	delete rightHeld;
}

bool Controls::raycast(Vector3& hitPos, Drawable*& hitDrawable, Camera* camera) {
	hitDrawable = nullptr;

	IntVector2 pos = Game::getInstance()->getUI()->GetCursorPosition();
	if (!Game::getInstance()->getUI()->GetCursor()->IsVisible() || Game::getInstance()->getUI()->GetElementAt(pos, true)) {
		return false;
	}

	Ray cameraRay = camera->GetScreenRay((float)pos.x_ / Game::getInstance()->getGraphics()->GetWidth(), (float)pos.y_ / Game::getInstance()->getGraphics()->GetHeight());

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	Game::getInstance()->getScene()->GetComponent<Octree>()->RaycastSingle(query);
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
		selected->at(i)->unSelect();
	}
	selected->clear();
}

void Controls::action(ActionType action, Entity* entity) {
	for (int i = 0; i < selected->size(); i++) {
		selected->at(i)->action(action, entity);
	}
}

void Controls::select(Entity* entity) {
	if (entity->getType() == selectedType) {

	} else {
		unSelect(entity->getType());
	}
	entity->select();
	selected->push_back(entity);
}

void Controls::click(int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::getInstance()->getCameraManager()->getComponent())) {
		Node* hitNode = hitDrawable->GetNode();
		switch (button) {
		case MOUSEB_LEFT:
			clickLeft(hitDrawable, hitPos);
			break;
		case MOUSEB_RIGHT:
			clickRight(hitDrawable, hitPos);
			break;
		}
	}
}

void Controls::clickLeft(Drawable* hitDrawable, Vector3 hitPos) {
	Node* hitNode = hitDrawable->GetNode();

	if (hitNode->GetName() == "Box") {
		LinkComponent* lc = hitNode->GetComponent<LinkComponent>();
		Entity* clicked = lc->getEntity();
		select(clicked);
	} else if (hitNode->GetName() == "Ground") {
		LinkComponent* lc = hitNode->GetComponent<LinkComponent>();
		Entity* clicked = lc->getEntity();
		select(clicked);
	}
}

void Controls::clickRight(Drawable* hitDrawable, Vector3 hitPos) {
	Node* hitNode = hitDrawable->GetNode();

	if (hitNode->GetName() == "Box") {
		unSelect(ENTITY);
	} else if (hitNode->GetName() == "Ground") {
		Entity* entity = new Entity(new Vector3(hitPos), nullptr, nullptr);
		Command* command = new Command(selected, ADD_AIM, entity);

		Game::getInstance()->getCommandList()->add(command);

		//action(ADD_AIM, entity);
	}
}

void Controls::leftReleased(std::pair<Entity*, Entity*>* held) {
	std::vector<Entity*>* entities = Game::getInstance()->getMediator()->getEntities(held);
	for (int i = 0; i < entities->size(); ++i) {//TODO zastapic wrzuceniem na raz
		select((*entities)[i]);
	}
	delete entities;
}

void Controls::release(const int button) {

	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::getInstance()->getCameraManager()->getComponent())) {
		Entity* entity = new Entity(&hitPos, nullptr, nullptr);
		switch (button) {
		case MOUSEB_LEFT:
			if (mouseLeftHeld == true) {
				mouseLeftHeld = false;
				leftHeld->second = entity;
				leftReleased(leftHeld);
			}
			break;
		case MOUSEB_RIGHT:
			if (mouseRightHeld == true) {
				mouseRightHeld = false;
				rightHeld->second = entity;
				//rightReleased(rightHeld);
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

void Controls::clickDownRight(Vector3 hitPos) {
	Entity* entity = new Entity(&hitPos, nullptr, nullptr);

	rightHeld->first = entity;
}

void Controls::clickDownLeft(Vector3 hitPos) {
	Entity* entity = new Entity(new Vector3(hitPos), nullptr, nullptr);

	leftHeld->first = entity;
}

void Controls::clickDown(const int button) {
	Vector3 hitPos;
	Drawable* hitDrawable;

	if (raycast(hitPos, hitDrawable, Game::getInstance()->getCameraManager()->getComponent())) {
		Node* hitNode = hitDrawable->GetNode();
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
