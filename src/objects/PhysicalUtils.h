#pragma once
#include "Physical.h"

bool belowClose(Physical* physical) {
	return physical->belowCloseLimit();
}

bool belowRange(Physical* physical) {
	return physical->belowRangeLimit();
}

Vector2 posToFollow(Physical* physical, Vector3* center) {
	return physical->getPosToFollow(center);
}

Vector2 exactPos(Physical* physical, Vector3* center) {
	return Vector2(physical->getPosition()->x_, physical->getPosition()->z_);
}
