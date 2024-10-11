#pragma once

#include <span>
#include <string>
#include <vector>

class Layer;
//TODO remember AVX2 set
class Brain {
public:
	explicit Brain(const std::string& filename, std::vector<std::string>& lines);
	Brain(const Brain& rhs) = delete;
	~Brain();

	const std::span<float> decide(std::span<const float> data);
	std::string getName() const;
private:
	std::vector<Layer*> allLayers;

	std::string filename;
};
