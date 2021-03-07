#pragma once
#include <fstream>
#include <string>
#include <vector>

inline std::vector<std::string> loadLines(const std::string& path) {
	std::ifstream infile(path);
	std::string data;
	std::vector<std::string> lines;
	while (std::getline(infile, data)) {
		lines.push_back(data);
	}
	infile.close();
	return lines;
}
