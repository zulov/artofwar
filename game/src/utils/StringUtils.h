#pragma once
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <Urho3D/Container/Str.h>
#include <fast_float/fast_float.h>
#include <Urho3D/Resource/Localization.h>

#include "Game.h"

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

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
Urho3D::String l10nFormat(const char* fmt, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) {
	return Urho3D::String().AppendWithFormat(Game::getLocalization()->Get(Urho3D::String(fmt)).CString(), t1, t2, t3, t4, t5, t6, t7, t8);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
Urho3D::String l10nFormat(const char* fmt, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) {
	return Urho3D::String().AppendWithFormat(Game::getLocalization()->Get(Urho3D::String(fmt)).CString(), t1, t2, t3, t4, t5, t6);
}

template <typename T1, typename T2>
Urho3D::String l10nFormat(const char* fmt, T1 t1, T2 t2) {
	return Urho3D::String().AppendWithFormat(Game::getLocalization()->Get(Urho3D::String(fmt)).CString(), t1,t2);
}

template <typename T1>
Urho3D::String l10nFormat(const char* fmt, T1 t1) {
	return Urho3D::String().AppendWithFormat(Game::getLocalization()->Get(Urho3D::String(fmt)).CString(), t1);
}
