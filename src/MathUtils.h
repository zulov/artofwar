#pragma once
#include "objects/Physical.h"

inline float sqDist(const Physical* one, const Physical* two) {
	return (*one->getPosition() - *two->getPosition()).LengthSquared();
}

inline float sqDist(const Vector3* one, const Vector3* two) {
	return (*one - *two).LengthSquared();
}

inline float sqDist(const Vector3& one, const Vector3& two) {
	return (one - two).LengthSquared();
}

inline float sqDist(const Vector2& one, const Vector3& two) {
	return Vector2(one.x_ - two.x_, one.y_ - two.z_).LengthSquared();
}


inline IntVector2 calculateSize(int size, int central) {
	const int first = -((size - 1) / 2);
	const int second = size + first;
	return IntVector2(first + central, second + central);
}
