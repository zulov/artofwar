#include "Controls.h"
#include "Game.h"


Controls::Controls() {
	selected = new std::vector<Entity*>();
	selected->reserve(10);
}


Controls::~Controls() {
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
