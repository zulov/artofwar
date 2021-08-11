#pragma once
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <span>

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


inline std::string asStringF(float val) {
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << val;
	return ss.str();
}
