#pragma once
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <span>
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

template <typename Iterator>
std::string join(Iterator begin, Iterator end, char separator = ';') {
	std::ostringstream o;
	if (begin != end) {
		o << *begin++;
		for (; begin != end; ++begin)
			o << separator << *begin;
	}
	return o.str(); //TODO performance czy to kopia?
}

template <typename T>
std::string join(std::span<T> span, char separator = ';') {
	return join(span.begin(), span.end(), separator);
}

template <typename T>
std::string join(std::vector<T> span, char separator = ';') {
	return join(span.begin(), span.end(), separator);
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
