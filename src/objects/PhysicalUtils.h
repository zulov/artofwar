#pragma once
#include "MathUtils.h"
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

inline std::tuple<Urho3D::Vector2, int> posToFollow(Physical* physical, Urho3D::Vector3* center) {
	return physical->getPosToFollowWithIndex(center);
}

inline std::tuple<Urho3D::Vector2, int> exactPos(Physical* physical, Urho3D::Vector3* center) {
	return {Urho3D::Vector2(physical->getPosition()->x_, physical->getPosition()->z_), -1};
}

std::tuple<Physical*, float, int> closestPhysical(Urho3D::Vector3* mainPos,
                                                  std::vector<Physical*>* things,
                                                  const std::function<bool(Physical*)>& condition,
                                                  const std::function<
	                                                  std::tuple<Urho3D::Vector2, int>(
	                                                                                   Physical * ,
	                                                                                   Urho3D::Vector3 * )>& position) {
	float minDistance = 99999;
	Physical* closest = nullptr;
	int bestIndex = -1;
	for (auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			auto [pos, indexOfPos] = position(entity, mainPos);
			const float distance = sqDist(pos, *mainPos);
			if (distance <= minDistance) {
				minDistance = distance;
				closest = entity;
				bestIndex = indexOfPos;
			}
		}
	}
	return std::tuple<Physical*, float, int>(closest, minDistance, bestIndex);
}
