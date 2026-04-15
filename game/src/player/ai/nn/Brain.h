#pragma once

#include <span>
#include <string>
#include <vector>

class Layer;
struct LayerData;
//TODO remember AVX2 set
class Brain {
public:
	explicit Brain(const std::string& filename, std::vector<LayerData>& layers);
	Brain(const Brain& rhs) = delete;
	~Brain();

	std::span<const float> decide(std::span<const float> data);
	std::string getName() const;
private:
	std::vector<Layer*> allLayers;

	std::string filename;
};
