#include "Brain.h"
#include "utils.h"

Brain::Brain(int numberOfHiddenLayers, int numberOfInput, int numberOfOutput, int wSize) {
	input = new Layer(numberOfInput, 0);
	output = new Layer(numberOfOutput, wSize);

	hidden.reserve(numberOfHiddenLayers);
	hidden.push_back(new Layer(wSize, numberOfInput));
	for (int i = 0; i < numberOfHiddenLayers; ++i) {
		hidden.push_back(new Layer(wSize, wSize));
	}

	allLayers.push_back(input);
	for (auto value : hidden) {
		allLayers.push_back(value);
	}
	allLayers.push_back(output);
}

Brain::~Brain() {
	clear_vector(allLayers);
}

double* Brain::decide(double data[]) {
	input->setValues(data);
	for (int i = 1; i < allLayers.size(); i++) {
		Layer* layer = allLayers.at(i);
		Layer* prevLayer = allLayers.at(i - 1);

		Eigen::MatrixXd mult = multiply(layer, prevLayer);

		setValues(layer, mult);
	}
	return output->getValues();
}

Eigen::MatrixXd Brain::multiply(Layer* current, Layer* prevLayer) {
	//TODO performance 
	auto input = Eigen::Map<Eigen::VectorXd>(prevLayer->getValues(), prevLayer->getNumberOfValues());
	auto weightedMatrix = Eigen::Map<Eigen::MatrixXd>(current->getW(), current->getPrevSize(),
	                                                  current->getNumberOfValues()).transpose();

	return weightedMatrix * input;
}

void Brain::setValues(Layer* layer, Eigen::MatrixXd& mult) const {
	for (int i = 0; i < mult.rows(); i++) {
		double q = mult(i) + layer->getBias(i);
		double newValue = tanh(q);
		layer->setValueAt(i, newValue);
	}
}