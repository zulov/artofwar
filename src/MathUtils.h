#pragma once
#include "objects/Physical.h"

inline float sqDist(const Physical* one, const Physical* two) {
	return (*one->getPosition() - *two->getPosition()).LengthSquared();
}

inline float sqDist(const Urho3D::Vector3* one, const Urho3D::Vector3* two) {
	return (*one - *two).LengthSquared();
}

inline float sqDist(const Urho3D::Vector2& one, const Urho3D::Vector2& two) {
	return (one - two).LengthSquared();
}

inline float sqDist(const Urho3D::Vector3& one, const Urho3D::Vector3& two) {
	return (one - two).LengthSquared();
}

inline float sqDist(const Urho3D::Vector2& one, const Urho3D::Vector3& two) {
	return Urho3D::Vector2(one.x_ - two.x_, one.y_ - two.z_).LengthSquared();
}

inline float sqDist(const Urho3D::Vector3& one, const Urho3D::Vector2& two) {
	return Urho3D::Vector2(one.x_ - two.x_, one.z_ - two.y_).LengthSquared();
}

inline Urho3D::IntVector2 calculateSize(int size, int central) {
	const int first = -((size - 1) / 2);
	const int second = size + first;
	return Urho3D::IntVector2(first + central, second + central);
}
