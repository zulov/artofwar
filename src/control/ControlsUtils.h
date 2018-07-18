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


inline void resultQuery(const Urho3D::Ray& cameraRay, Urho3D::PODVector<Urho3D::RayQueryResult>& results) {
	Urho3D::RayOctreeQuery query(results, cameraRay, Urho3D::RAY_TRIANGLE, 300, Urho3D::DRAWABLE_GEOMETRY);
	Game::getScene()->Node::GetComponent<Urho3D::Octree>()->Raycast(query);
}

bool raycast(hit_data& hitData) {
	const auto pos = Game::getUI()->GetCursorPosition();
	if (!Game::getUI()->GetCursor()->IsVisible() || Game::getUI()->GetElementAt(pos, true)) {
		return false;
	}

	const auto cameraRay = Game::getCameraManager()
	                                 ->getComponent()
	                                 ->GetScreenRay((float)pos.x_ / Game::getGraphics()->GetWidth(),
	                                                (float)pos.y_ / Game::getGraphics()->GetHeight());

	Urho3D::PODVector<Urho3D::RayQueryResult> results;
	resultQuery(cameraRay, results);
	int i = 0;
	while (i < results.Size()) {
		Urho3D::RayQueryResult& result = results[i];

		auto node = result.node_;

		auto lc = node->GetComponent<LinkComponent>();
		if (lc) {
			hitData.set(result, lc);
			return true;
		}
		lc = node->GetParent()->GetComponent<LinkComponent>();
		if (lc) {
			hitData.set(result, lc);
			return true;
		}
		++i;
	}
	hitData.drawable = nullptr;
	hitData.link = nullptr;

	return false;
}

bool raycast(hit_data& hitData, ObjectType type) {
	const auto pos = Game::getUI()->GetCursorPosition();
	if (!Game::getUI()->GetCursor()->IsVisible() || Game::getUI()->GetElementAt(pos, true)) {
		return false;
	}

	const auto cameraRay = Game::getCameraManager()
	                                 ->getComponent()
	                                 ->GetScreenRay((float)pos.x_ / Game::getGraphics()->GetWidth(),
	                                                (float)pos.y_ / Game::getGraphics()->GetHeight());

	Urho3D::PODVector<Urho3D::RayQueryResult> results;
	resultQuery(cameraRay, results);
	int i = 0;
	while (i < results.Size()) {
		Urho3D::RayQueryResult& result = results[i];

		auto node = result.node_;

		auto lc = node->GetComponent<LinkComponent>();
		if (lc && lc->getPhysical()->getType() == type) {
			hitData.set(result, lc);
			return true;
		}
		lc = node->GetParent()->GetComponent<LinkComponent>();
		if (lc && lc->getPhysical()->getType() == type) {
			hitData.set(result, lc);
			return true;
		}
		++i;
	}
	hitData.drawable = nullptr;
	hitData.link = nullptr;

	return false;
}
