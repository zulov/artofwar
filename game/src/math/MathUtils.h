#pragma once
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Vector2.h>
#include <array>
#include <valarray>

inline Urho3D::IntVector2 calculateSize(int size, int central) {
	const int first = -((size - 1) / 2);
	const int second = size + first;
	return {first + central, second + central};
}

inline void setClosest(float& minDistance, Urho3D::Vector2& closest, int& closestindex, int i,
                       Urho3D::Vector2 posToFollow, const Urho3D::Vector2& pos1) {
	auto dist = pos1.SqDistXZ(posToFollow);

	if (dist < minDistance) {
		minDistance = dist;
		closest = posToFollow;
		closestindex = i;
	}
}

inline short diff(const short a, const short b) {
	auto diff = Urho3D::Sign(b - a);
	if (diff == 0) { return 1; }
	return diff;
}

inline float fixValue(float value, float maxValue) {
	if (value > maxValue) {
		return maxValue;
	}
	if (value < 0) {
		return 0;
	}
	return value;
}

inline std::array<int, 4> getCordsInHigher(unsigned short resolution, int index) {
	const auto mod = index % resolution;
	const auto div = index / resolution;
	const auto resX2 = resolution * 2;
	const auto value = 2 * (mod + div * resX2);

	return {value, value + 1, value + resX2, value + resX2 + 1};
}

inline int getCordsInLower(int currentRes, int parentRes, int index) {
	auto x = index / parentRes;
	auto z = index % parentRes;
	x /= 2;
	z /= 2;
	return x * currentRes + z;
}

inline float sqRootSumError(const std::valarray<float>& val1, const std::valarray<float>& val2) {
	auto diff = val1 - val2;
	diff *= diff;
	return diff.sum();
}