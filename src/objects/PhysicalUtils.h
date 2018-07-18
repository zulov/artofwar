#pragma once
#include "Physical.h"

inline bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

inline bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

inline bool alwaysTrue(Physical* physical) {
	return true;
}

inline Urho3D::Vector2 posToFollow(Physical* physical, Urho3D::Vector3* center) {
	return physical->getPosToFollow(center);
}

inline Urho3D::Vector2 exactPos(Physical* physical, Urho3D::Vector3* center) {
	return Urho3D::Vector2(physical->getPosition()->x_, physical->getPosition()->z_);
}

std::tuple<Physical*, float> closestPhysical(Physical* physical,
                                             std::vector<Physical*>* things,
                                             const std::function<bool(Physical*)>& condition,
                                             const std::function<Urho3D::Vector2(Physical*, Urho3D::Vector3*)>& position) {
	float minDistance = 99999;
	Physical* closest = nullptr;
	for (auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			const float distance = sqDist(position(entity, physical->getPosition()), *physical->getPosition());
			if (distance <= minDistance) {
				minDistance = distance;
				closest = entity;
			}
		}
	}
	return {closest, minDistance};
}
