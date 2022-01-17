#pragma once

#include <span>
#include <string>
#include <vector>
#include <Eigen/Core>

class Layer;

class Brain {
public:
	explicit Brain(const std::string& filename, std::vector<std::string>& lines);
	Brain(const Brain& rhs) = delete;
	~Brain();

	const std::span<float> decide(std::span<float> data);
	std::string getName() const;
	short getOutputSize() const;
private:
	void setValues(Layer* layer, Eigen::MatrixXf& mult) const;
	std::vector<Layer*> allLayers;

	std::string filename;
};
