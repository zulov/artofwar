#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

inline void loadLines(const std::string& path, std::vector<std::string>& lines) {
	std::ifstream infile(path, std::fstream::in);
	std::string data;
	lines.clear();
	while (std::getline(infile, data)) {
		lines.push_back(std::move(data));
	}
	infile.close();
}
