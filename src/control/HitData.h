#pragma once
#include <Urho3D/Graphics/OctreeQuery.h>

class Physical;

struct hit_data {
	hit_data() = default;
	hit_data(const hit_data&) = delete;
	Urho3D::Vector3 position;
	Urho3D::Drawable* drawable;
	Physical* clicked;

	void set(const Urho3D::RayQueryResult& result, Physical* lc) {
		position = result.position_;
		drawable = result.drawable_;
		clicked = lc;
	}
};
