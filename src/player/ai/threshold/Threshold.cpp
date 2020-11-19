#include "Threshold.h"

#include "simulation/env/influence/CenterType.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"

constexpr float THRESHOLD_VAL = 1.f;

Threshold::Threshold(std::string filename): filename(filename) {
	auto lines = loadLines("Data/ai/threshold/" + filename);
	data.reserve(lines.size());
	for (int i = 0; i < lines.size(); ++i) {
		auto splitVec = split(lines[i], ';');
		std::vector<float> vector;
		vector.reserve(splitVec.size());

		for (auto& str : splitVec) {
			vector.push_back(atof(str.c_str()));
		}
		data.emplace_back(vector);
	}
}

bool Threshold::hasReach(std::vector<float>& vec, std::span<float> value) const {
	assert(vec.size()==value.size());
	for (auto i = 0; i < value.size(); ++i) {
		const auto d = vec[i] - value[i];
		if (d > THRESHOLD_VAL) {
			return false;
		}
	}
	return true;
}

float Threshold::diff(std::vector<float>& vec, std::span<float> value) const {
	assert(vec.size()==value.size());
	float sum = 0.f;
	for (auto i = 0; i < value.size(); ++i) {
		const auto d = vec[i] - value[i];
		if (d > 0.f) {
			sum += d * d;
		} else {
			sum -= d;
		}
	}
	return sum;
}

bool Threshold::ifDo(std::span<float> value) {
	for (int i = 0; i < data.size(); ++i) {
		if (hasReach(data[i], value)) {
			return true;
		}
	}
	return false;
}

char Threshold::getBest(std::span<float> value) {
	char best = 0;
	float bestVal = diff(data[0], value);

	for (char i = 1; i < data.size(); ++i) {
		const auto val = diff(data[i], value);
		if (val < bestVal) {
			bestVal = val;
			best = i;
		}
	}

	return best;
}

std::string Threshold::getName() const {
	return filename;
}
