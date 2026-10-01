#pragma once

#include <span>
#include <string>
#include <vector>

class Layer;
struct LayerData;
//TODO remember AVX2 set
class Brain {
public:
	explicit Brain(std::string filename, const std::vector<LayerData>& layers);
	Brain(const Brain& rhs) = delete;
	~Brain();

	std::span<const float> decide(std::span<const float> data);
	const std::string& getName() const;
	int getInputSize() const;
	int getOutputSize() const;
private:
	std::vector<Layer*> allLayers;

	std::string filename;
};
