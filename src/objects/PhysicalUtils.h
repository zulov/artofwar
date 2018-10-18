#pragma once
#include "MathUtils.h"


inline bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

inline bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

inline bool alwaysTrue(Physical* physical) {
	return true;
}

inline std::tuple<Urho3D::Vector2, float, int> posToFollow(Physical* toFollow, Unit* follower) {
	return toFollow->getPosToUseWithIndex(follower);
}
