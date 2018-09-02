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
