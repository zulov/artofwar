#pragma once

#include <magic_enum.hpp>
#include <span>
#include <vector>

class Threshold {
public:
	Threshold(const std::string& filename, std::vector<std::string>& lines);
	Threshold(const Threshold& rhs) = delete;

	bool ifDo(std::span<float> value);
	char getBest(std::span<float> value);
	std::string getName() const;
private:
	bool hasReach(std::vector<float>& vec, std::span<float> value) const;
	float diff(std::vector<float>& vec, std::span<float> value) const;

	std::vector<std::vector<float>> data; //TODO uproœciæ to do dwuwymiarowej tablicy, jedno wymiarowa i spany?

	std::string filename;
};
