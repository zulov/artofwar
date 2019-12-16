#pragma once
#include "Layer.h"
#include <vector>
#include <Eigen/Dense>

class Brain {
public:
	Brain(int numberOfHiddenLayers, int numberOfInput, int numberOfOutput, int wSize);
	double* decide(double data[]);
	Eigen::MatrixXd multiply(Layer* current, Layer* prevLayer);
	Eigen::MatrixXd columnsFromNode(Layer& layer);
	void setValues(Layer* layer, Eigen::MatrixXd& mult) const; 
private:

	Layer input;
	Layer output;
	std::vector<Layer> hidden;
	std::vector<Layer*> allLayers;

};
