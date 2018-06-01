#pragma once
#include "objects/Physical.h"

inline float sqDist(const Physical* one, const Physical* two) {
	return (*one->getPosition() - *two->getPosition()).LengthSquared();
}
