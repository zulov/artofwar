#include "Layer.h"
#include <algorithm>
#include <iostream>


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
