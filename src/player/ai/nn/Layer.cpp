#include "Layer.h"
#include <algorithm>
#include <iostream>

Layer::Layer(short numberOfNodes, short prevSize): numberOfNodes(numberOfNodes), prevSize(prevSize) {
	values = new float[numberOfNodes];
	std::fill_n(values, numberOfNodes, 0);

	bias = new float[numberOfNodes];
	for (int i = 0; i < numberOfNodes; i++) {
		bias[i] = (static_cast<float>(rand()) / RAND_MAX - 0.5) * 2;
	}
	w = new float[numberOfNodes * prevSize];
	for (int i = 0; i < numberOfNodes * prevSize; i++) {
		w[i] = (static_cast<float>(rand()) / RAND_MAX - 0.5) * 2;
	}

}

Layer::Layer(std::vector<float>& w, std::vector<float>& bias) {
	numberOfNodes = bias.size();
	prevSize = w.size() / bias.size();
	values = new float[bias.size()];
	std::fill_n(values, numberOfNodes, 0);

	this->bias = new float[bias.size()];
	std::copy(bias.begin(), bias.end(), this->bias);

	this->w = new float[w.size()];
	std::copy(w.begin(), w.end(), this->w);
}

Layer::~Layer() {
	delete []values;
	delete []bias;
	delete []w;
}

void Layer::setValues(float data[]) const {
	//TODO musi sie zgadzac rozmiar czy to jest sprawdzane?
	std::copy(data, data + numberOfNodes, values);
}
