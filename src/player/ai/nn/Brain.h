#pragma once

#include <span>
#include <string>
#include <vector>
#include <Eigen/Core>

class Layer;

class Brain {
public:
	explicit Brain(std::string filename);
	Brain(const Brain& rhs) = delete;
	~Brain();
	
	const std::span<float> decide(std::span<float> data);
	std::string getName() const;
private:
	void setValues(Layer* layer, Eigen::MatrixXf& mult) const;
	std::vector<Layer*> allLayers;

	std::string filename;
};
