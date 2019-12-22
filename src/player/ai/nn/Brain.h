#pragma once
#include "Layer.h"
#include <Eigen/Dense>

class Brain {
public:
	Brain(int numberOfHiddenLayers, int numberOfInput, int numberOfOutput, int wSize);
	explicit Brain(std::vector<std::string>& lines);
	~Brain();
	double* decide(double data[]);
	short getOutputSize() const { return output->getNumberOfValues(); }

private:
	Eigen::MatrixXd multiply(Layer* current, Layer* prevLayer);
	void setValues(Layer* layer, Eigen::MatrixXd& mult) const;
	Layer* input;
	Layer* output;
	std::vector<Layer*> hidden;
	std::vector<Layer*> allLayers;

};
