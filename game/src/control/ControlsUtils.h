#pragma once
#include "Game.h"
#include "HitData.h"
#include "camera/CameraManager.h"
#include "objects/Physical.h"
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/UI/UI.h>


inline Physical* getLink(Urho3D::Node* node) {
	return static_cast<Physical*>(node->GetVar("link").GetVoidPtr());
}

inline bool isGround(Urho3D::Node* node) {
	return node->GetVar("ground").GetBool() || (node->GetParent() && node->GetParent()->GetVar("ground").GetBool());
}

inline void resultQuery(const Urho3D::IntVector2& pos, Urho3D::PODVector<Urho3D::RayQueryResult>& results) {
	auto [width, height] = Game::getGraphics()->GetSize();
	const auto cameraRay = Game::getCameraManager()->getComponent()
	                                                ->GetScreenRay((float)pos.x_ / width, (float)pos.y_ / height);


	Urho3D::RayOctreeQuery query(results, cameraRay, Urho3D::RAY_TRIANGLE, 300, Urho3D::DRAWABLE_GEOMETRY);
	Game::getScene()->GetComponent<Urho3D::Octree>()->Raycast(query);
}

inline bool skipRayCast(const Urho3D::IntVector2 pos) {
	auto ui = Game::getUI();
	return !ui->GetCursor()->IsVisible() || ui->GetElementAt(pos, true);
}

bool raycast(hit_data& hitData) {
	const auto pos = Game::getUI()->GetCursorPosition();
	if (skipRayCast(pos)) {
		return false;
	}

	Urho3D::PODVector<Urho3D::RayQueryResult> results;
	resultQuery(pos, results);

	for (const auto &result : results) {
		const auto node = result.node_;
		if (isGround(node)) {
			hitData.set(result, true);
			return true;
		}
		if (Physical * lc; (lc = getLink(node)) || (lc = getLink(node->GetParent()))) {
			hitData.set(result, lc);
			return true;
		}
	}
	hitData.reset();
	return false;
}

bool raycastGround(hit_data& hitData) {
	const auto pos = Game::getUI()->GetCursorPosition();
	if (skipRayCast(pos)) {
		return false;
	}

	Urho3D::PODVector<Urho3D::RayQueryResult> results;
	resultQuery(pos, results);
	for (const auto result : results) {
		if (isGround(result.node_)) {
			hitData.set(result, true);
			return true;
		}
	}
	hitData.reset();
	return false;
}
