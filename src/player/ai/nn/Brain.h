#pragma once
#include "Layer.h"
#include <Eigen/Dense>
#include <span>

class Brain {
public:
	explicit Brain(std::string dataPath);
	Brain(const Brain& rhs) = delete;
	~Brain();
	const std::span<float> decide(std::span<float> data); //TODO std::span after c++20

private:
	Eigen::MatrixXf multiply(Layer* current, Layer* prevLayer);
	void setValues(Layer* layer, Eigen::MatrixXf& mult) const;
	Layer* input;
	Layer* output;
	std::vector<Layer*> allLayers;
};
