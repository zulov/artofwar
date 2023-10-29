#pragma once

#include <span>
#include <string>
#include <vector>

class Layer;

class Brain {
public:
	explicit Brain(const std::string& filename, std::vector<std::string>& lines);
	Brain(const Brain& rhs) = delete;
	~Brain();

	const std::span<float> decide(std::span<float> data);
	std::string getName() const;
private:
	float toFloat(const std::string &token);
	std::vector<Layer*> allLayers;

	std::string filename;
};
