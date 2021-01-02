#pragma once
#include <Urho3D/Graphics/OctreeQuery.h>

#include "objects/Physical.h"

class Physical;

struct hit_data {
	hit_data() = default;
	hit_data(const hit_data&) = delete;
	Urho3D::Drawable* drawable = nullptr;
	Physical* clicked = nullptr;
	Urho3D::Vector3 position;
	bool isGround = false;

	void set(const Urho3D::RayQueryResult& result, Physical* lc) {
		position = result.position_;
		drawable = result.drawable_;
		clicked = lc;
	}

	void set(const Urho3D::RayQueryResult& result, bool isGround) {
		position = result.position_;
		drawable = result.drawable_;
		this->isGround = isGround;
	}

	ObjectType getType() const {
		if (isGround || clicked == nullptr) {
			return ObjectType::NONE;
		}
		return clicked->getType();
	}

	bool isSth() const {
		return isGround || clicked;
	}
};
