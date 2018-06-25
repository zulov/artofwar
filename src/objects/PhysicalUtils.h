#pragma once
#include "Physical.h"

bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

bool alwaysTrue(Physical* physical) {
	return true;
}

Vector2 posToFollow(Physical* physical, Vector3* center) {
	return physical->getPosToFollow(center);
}

Vector2 exactPos(Physical* physical, Vector3* center) {
	return Vector2(physical->getPosition()->x_, physical->getPosition()->z_);
}

std::tuple<Physical*, float> closestPhysical(Physical* physical,
                                             std::vector<Physical*>* things,
                                             const std::function<bool(Physical*)>& condition,
                                             const std::function<Vector2(Physical*, Vector3*)>& position) {
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
