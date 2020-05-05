#pragma once
#include "Layer.h"
#include <Eigen/Dense>

class Brain {
public:
	explicit Brain(std::string dataPath);
	~Brain();
	const std::vector<float>& decide(float data[]);//TODO std::spac after c++20

private:
	Eigen::MatrixXf multiply(Layer* current, Layer* prevLayer);
	void setValues(Layer* layer, Eigen::MatrixXf& mult) const;
	Layer* input;
	Layer* output;
	std::vector<Layer*> allLayers;
	std::vector<float> result;
};
