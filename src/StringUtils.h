#pragma once
#include <string>
#include <vector>
#include <sstream>

inline std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;//TODO performance czy to kopia?
}

template <typename Iterator>
std::string join(Iterator begin, Iterator end, char separator = ';') {
	std::ostringstream o;
	if (begin != end) {
		o << *begin++;
		for (; begin != end; ++begin)
			o << separator << *begin;
	}
	return o.str();//TODO performance czy to kopia?
}