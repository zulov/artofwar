#pragma once
#include "objects/LinkComponent.h"
#include <Urho3D/Graphics/Drawable.h>
#include <Urho3D/Graphics/OctreeQuery.h>


struct hit_data
{
	Urho3D::Vector3 position;
	Urho3D::Drawable* drawable;
	LinkComponent* link;

	void set(const Urho3D::RayQueryResult& result, LinkComponent* lc) {
		position = result.position_;
		drawable = result.drawable_;
		link = lc;
	}
};
