#pragma once
#include "Layer.h"
#include <Eigen/Dense>

class Brain {
public:
	explicit Brain(std::string dataPath);
	~Brain();
	float* decide(float data[]);
	short getOutputSize() const { return output->getNumberOfValues(); }

private:
	Eigen::MatrixXf multiply(Layer* current, Layer* prevLayer);
	void setValues(Layer* layer, Eigen::MatrixXf& mult) const;
	Layer* input;
	Layer* output;
	std::vector<Layer*> hidden;
	std::vector<Layer*> allLayers;

};
