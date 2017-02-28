#include "Controls.h"



Controls::Controls(UI* _ui, Graphics* _graphics) {
	ui = _ui;
	graphics = _graphics;
	selected = new std::vector<Entity*>();
	selected->reserve(10);
}


Controls::~Controls() {}



bool Controls::raycast(float maxDistance, Vector3& hitPos, Drawable*& hitDrawable, Camera* camera, Scene * scene) {
	hitDrawable = 0;

	IntVector2 pos = ui->GetCursorPosition();
	// Check the cursor is visible and there is no UI element in front of the cursor
	if (!ui->GetCursor()->IsVisible() || ui->GetElementAt(pos, true)) {
		return false;
	}

	Ray cameraRay = camera->GetScreenRay((float)pos.x_ / graphics->GetWidth(), (float)pos.y_ / graphics->GetHeight());

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	scene->GetComponent<Octree>()->RaycastSingle(query);
	if (results.Size()) {
		RayQueryResult& result = results[0];
		hitPos = result.position_;
		hitDrawable = result.drawable_;
		return true;
	}

	return false;
}

void Controls::select(Entity * entity) {
	if(entity->getType()== selectedType) {
		
	}else {
		selected->clear();
	}
	selected->push_back(entity);
}


