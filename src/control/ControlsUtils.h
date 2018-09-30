#pragma once
#include "Game.h"
#include "HitData.h"
#include "camera/CameraManager.h"
#include "objects/Physical.h"
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/UI/UI.h>


inline Physical* getLink(Urho3D::Node* node) {
	return static_cast<Physical*>(node->GetVar("link").GetVoidPtr());
}

inline void resultQuery(const Urho3D::IntVector2& pos, Urho3D::PODVector<Urho3D::RayQueryResult>& results) {
	const auto& cameraRay = Game::getCameraManager()->getComponent()
	                                                ->GetScreenRay((float)pos.x_ / Game::getGraphics()->GetWidth(),
	                                                               (float)pos.y_ / Game::getGraphics()->GetHeight());

	Urho3D::RayOctreeQuery query(results, cameraRay, Urho3D::RAY_TRIANGLE, 300, Urho3D::DRAWABLE_GEOMETRY);
	Game::getScene()->Node::GetComponent<Urho3D::Octree>()->Raycast(query);
}

bool raycast(hit_data& hitData) {
	const auto pos = Game::getUI()->GetCursorPosition();
	if (!Game::getUI()->GetCursor()->IsVisible() || Game::getUI()->GetElementAt(pos, true)) {
		return false;
	}

	Urho3D::PODVector<Urho3D::RayQueryResult> results;
	resultQuery(pos, results);

	for (const auto result : results) {
		const auto node = result.node_;
		Physical* lc;
		if ((lc = getLink(node)) || (lc = getLink(node->GetParent()))) {
			hitData.set(result, lc);
			return true;
		}
	}
	hitData.drawable = nullptr;
	hitData.clicked = nullptr;

	return false;
}

bool raycast(hit_data& hitData, ObjectType type) {
	const auto pos = Game::getUI()->GetCursorPosition();
	if (!Game::getUI()->GetCursor()->IsVisible() || Game::getUI()->GetElementAt(pos, true)) {
		return false;
	}

	Urho3D::PODVector<Urho3D::RayQueryResult> results;
	resultQuery(pos, results);
	for (const auto result : results) {
		const auto node = result.node_;
		Physical* lc;
		if ((lc = getLink(node)) && lc->getType() == type
			|| (lc = getLink(node->GetParent())) && lc->getType() == type) {
			hitData.set(result, lc);
			return true;
		}
	}
	hitData.drawable = nullptr;
	hitData.clicked = nullptr;

	return false;
}
