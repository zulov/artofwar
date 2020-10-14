#include "Brain.h"

#include <fstream>
#include <iostream>
#include "AFUtil.h"
#include "utils/DeleteUtils.h"
#include "utils/StringUtils.h"

Brain::Brain(std::string filename) {
	std::ifstream infile("Data/ai/" + filename);
	std::string data;
	std::vector<std::string> lines;
	while (std::getline(infile, data)) {
		lines.push_back(data);
	}
	infile.close();

	std::vector<float> w;
	std::vector<float> b;
	for (auto& line : lines) {
		auto splitVec = split(line, ';');
		auto p = std::find(splitVec.begin(), splitVec.end(), "");
		std::vector<std::string>::iterator i;
		for (i = splitVec.begin(); i != p; ++i) {
			w.push_back(atof((*i).c_str()));
		}
		i += 2;
		for (; i != splitVec.end(); ++i) {
			b.push_back(atof((*i).c_str()));
		}
		allLayers.push_back(new Layer(w, b));
		w.clear();
		b.clear();
	}
}

Brain::~Brain() {
	clear_vector(allLayers);
}

const std::span<float> Brain::decide(std::span<float> data) {
	allLayers.front()->setValues(data);
	for (int i = 1; i < allLayers.size(); i++) {
		Layer* layer = allLayers.at(i);
		Layer* prevLayer = allLayers.at(i - 1);

		Eigen::MatrixXf mult = multiply(layer, prevLayer);

		setValues(layer, mult);
	}

	return allLayers.back()->getValues();
}

Eigen::MatrixXf Brain::multiply(Layer* current, Layer* prevLayer) {
	//TODO performance
	const auto vals = prevLayer->getValues();
	
	const auto input = Eigen::Map<Eigen::VectorXf>(vals.data(), vals.size());
	const auto weightedMatrix = Eigen::Map<Eigen::MatrixXf>(current->getW(), current->getPrevSize(),
	                                                        current->getNumberOfValues()).transpose();

	return weightedMatrix * input;
}

void Brain::setValues(Layer* layer, Eigen::MatrixXf& mult) const {
	for (int i = 0; i < mult.rows(); i++) {
		double q = mult(i) + layer->getBias(i);
		double newValue = tanh1(q);
		layer->setValueAt(i, newValue);
	}
}
