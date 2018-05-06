#pragma once
#include "Game.h"
#include "HitData.h"
#include "camera/CameraManager.h"
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>


bool raycast(hit_data& hitData) {
	const IntVector2 pos = Game::get()->getUI()->GetCursorPosition();
	if (!Game::get()->getUI()->GetCursor()->IsVisible() || Game::get()->getUI()->GetElementAt(pos, true)) {
		return false;
	}
	Camera* camera = Game::get()->getCameraManager()->getComponent();
	const Ray cameraRay = camera->GetScreenRay((float)pos.x_ / Game::get()->getGraphics()->GetWidth(),
	                                           (float)pos.y_ / Game::get()->getGraphics()->GetHeight());

	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, 300, DRAWABLE_GEOMETRY);
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


void createNode(const String& model, const String& texture, Node** node) {
	*node = Game::get()->getScene()->CreateChild();
	auto selectionModel = (*node)->CreateComponent<StaticModel>();
	selectionModel->SetModel(Game::get()->getCache()->GetResource<Model>(model));
	selectionModel->SetMaterial(Game::get()->getCache()->GetResource<Material>(texture));
	(*node)->SetEnabled(false);
}