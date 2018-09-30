#pragma once
#include <Urho3D/Graphics/Drawable.h>
#include <Urho3D/Graphics/OctreeQuery.h>

class Physical;

struct hit_data
{
	Urho3D::Vector3 position;
	Urho3D::Drawable* drawable;
	Physical* clicked;

	void set(const Urho3D::RayQueryResult& result, Physical* lc) {
		position = result.position_;
		drawable = result.drawable_;
		clicked = lc;
	}
};
