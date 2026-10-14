#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace RoundUtils {

template <typename Integer>
inline Integer roundClamped(float value, Integer minimum, Integer maximum) {
	if (std::isnan(value)) return minimum;

	const auto rounded = std::round(static_cast<double>(value));
	return static_cast<Integer>(std::clamp(rounded, static_cast<double>(minimum), static_cast<double>(maximum)));
}

inline signed char toChar(float value) {
	return roundClamped(value, std::numeric_limits<signed char>::lowest(), std::numeric_limits<signed char>::max());
}

inline signed char toChar(float value, signed char minimum) {
	return roundClamped(value, minimum, std::numeric_limits<signed char>::max());
}

inline unsigned char toUChar(float value) {
	return roundClamped(value, static_cast<unsigned char>(0), std::numeric_limits<unsigned char>::max());
}

inline unsigned char toUChar(float value, unsigned char minimum) {
	return roundClamped(value, minimum, std::numeric_limits<unsigned char>::max());
}

inline short toShort(float value) {
	return roundClamped(value, std::numeric_limits<short>::lowest(), std::numeric_limits<short>::max());
}

inline short toShort(float value, short minimum) {
	return roundClamped(value, minimum, std::numeric_limits<short>::max());
}

inline int toInt(float value) {
	return roundClamped(value, std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max());
}

inline int toInt(float value, int minimum) {
	return roundClamped(value, minimum, std::numeric_limits<int>::max());
}

inline unsigned int toUInt(float value) {
	return roundClamped(value, 0u, std::numeric_limits<unsigned int>::max());
}

inline unsigned int toUInt(float value, unsigned int minimum) {
	return roundClamped(value, minimum, std::numeric_limits<unsigned int>::max());
}

inline unsigned short toUShort(float value) {
	return roundClamped(value, static_cast<unsigned short>(0), std::numeric_limits<unsigned short>::max());
}

inline unsigned short toUShort(float value, unsigned short minimum) {
	return roundClamped(value, minimum, std::numeric_limits<unsigned short>::max());
}

}
