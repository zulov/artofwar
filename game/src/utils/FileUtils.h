#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <fast_float/fast_float.h>

struct LayerData {
	std::vector<float> weights;
	std::vector<float> biases;
};

inline bool loadBrainFile(const std::string& path, std::vector<LayerData>& layers, std::string& buffer) {
	std::ifstream infile(path, std::ios::in | std::ios::ate | std::ios::binary);
	if (!infile.is_open()) {
		return false;
	}

	const auto fileSize = infile.tellg();
	if (fileSize < 0) {
		return false;
	}
	infile.seekg(0, std::ios::beg);

	buffer.resize(static_cast<size_t>(fileSize));
	if (!buffer.empty() && !infile.read(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
		return false;
	}

	layers.clear();

	const char* ptr = buffer.data();
	const char* end = ptr + buffer.size();

	while (ptr < end) {
		LayerData layer;
		bool parsing_weights = true;

		if (ptr < end && *ptr == ';') {
			parsing_weights = false;
			ptr++;
		}

		while (ptr < end && *ptr != '\n' && *ptr != '\r') {
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
				ptr++;
				continue;
			}

			if (parsing_weights) {
				layer.weights.push_back(val);
			} else {
				layer.biases.push_back(val);
			}

			ptr = result.ptr;
		}

		while (ptr < end && (*ptr == '\n' || *ptr == '\r')) {
			ptr++;
		}

		if (!layer.weights.empty() || !layer.biases.empty()) {
			layers.push_back(std::move(layer));
		}
	}
	return true;
}

inline bool loadBrainFile(const std::string& path, std::vector<LayerData>& layers) {
	std::string buffer;
	return loadBrainFile(path, layers, buffer);
}
