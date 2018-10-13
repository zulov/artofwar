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

inline std::tuple<Urho3D::Vector2, int> posToFollow(Physical* toFollow, Unit* follower) {
	return toFollow->getPosToUseWithIndex(follower);
}

inline std::tuple<Urho3D::Vector2, int> exactPos(Physical* toFollow, Unit* follower) {
	return {Urho3D::Vector2(toFollow->getPosition()->x_, toFollow->getPosition()->z_), -1};
}
