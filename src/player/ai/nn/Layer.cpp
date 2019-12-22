#include "Layer.h"
#include <algorithm>
#include <iostream>

Layer::Layer(short numberOfNodes, short prevSize): numberOfNodes(numberOfNodes), prevSize(prevSize) {
	values = new double[numberOfNodes];
	std::fill_n(values, numberOfNodes, 0);

	bias = new double[numberOfNodes];
	for (int i = 0; i < numberOfNodes; i++) {
		bias[i] = (static_cast<float>(rand()) / RAND_MAX - 0.5) * 2;
	}
	w = new double[numberOfNodes * prevSize];
	for (int i = 0; i < numberOfNodes * prevSize; i++) {
		w[i] = (static_cast<float>(rand()) / RAND_MAX - 0.5) * 2;
	}

}

Layer::Layer(std::vector<double>& w, std::vector<double>& bias) {
	values = new double[bias.size()];
	std::fill_n(values, numberOfNodes, 0);

	this->bias = new double[bias.size()];
	std::copy(bias.begin(), bias.end(), this->bias);

	this->w = new double[w.size()];
	std::copy(w.begin(), w.end(), this->w);
	numberOfNodes = bias.size();
	prevSize = w.size() / bias.size();
}

Layer::~Layer() {
	delete []values;
	delete []bias;
	delete []w;
}

void Layer::setValues(double data[]) {
	//TODO musi sie zgadzac rozmiar czy to jest sprawdzane?
	std::copy(data, data + numberOfNodes, values);
}
