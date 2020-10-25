#pragma once

#include <Eigen/Dense>
#include <span>

class Layer;

class Brain {
public:
	explicit Brain(std::string filename);
	Brain(const Brain& rhs) = delete;
	~Brain();
	const std::span<float> decide(std::span<float> data);

private:
	Eigen::MatrixXf multiply(Layer* current, Layer* prevLayer);
	void setValues(Layer* layer, Eigen::MatrixXf& mult) const;
	std::vector<Layer*> allLayers;
};
