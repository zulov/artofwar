#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <fast_float/fast_float.h>

inline void loadLines(const std::string& path, std::vector<std::string>& lines) {
	std::ifstream infile(path, std::fstream::in);
	std::string data;
	lines.clear();
	while (std::getline(infile, data)) {
		lines.push_back(std::move(data));
	}
	infile.close();
}

//TODO przeanalizowac to
inline void parseLine(const std::string& line, std::vector<float>& w, std::vector<float>& b) {
	const char* ptr = line.data();
	const char* end = ptr + line.size();

	bool parsing_weights = true;

	// ✅ handle leading ';' → no weights
	if (ptr < end && *ptr == ';') {
		parsing_weights = false;
		ptr++;
	}

	while (ptr < end) {
		// detect section switch (;;)
		if (*ptr == ';') {
			if (ptr + 1 < end && ptr[1] == ';') {
				parsing_weights = false;
				ptr += 2;
				continue;
			}
			ptr++;
			continue;
		}

		float val;
		auto result = fast_float::from_chars(ptr, end, val);

		if (result.ec != std::errc()) {
			// skip invalid char
			ptr++;
			continue;
		}

		if (parsing_weights) {
			w.push_back(val);
		} else {
			b.push_back(val);
		}

		ptr = result.ptr;
	}
}
