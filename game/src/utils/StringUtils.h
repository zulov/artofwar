#pragma once
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <Urho3D/Container/Str.h>
#include <fast_float/fast_float.h>

inline std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens; //TODO performance czy to kopia?
}

inline std::string asStringF(float val, int precision = 2) {
	std::stringstream ss;
	ss << std::fixed << std::setprecision(precision) << val;
	return ss.str();
}

inline int fatoi(const char* str) {
	int val = 0;
	while (*str) {
		val = val * 10 + (*str++ - '0');
	}
	return val;
}

inline float toFloat(const std::string& token) {
	float f;
	fast_float::from_chars(token.data(), token.data() + token.size(), f);
	return f;
}

inline Urho3D::String to2DigString(unsigned char v) {
	return (v < 10 ? "0" : "") + Urho3D::String((unsigned int)v);
}
