#include "Layer.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <span>


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

void Layer::setValues(std::span<float> data) const {
	assert(numberOfNodes == data.size());
	std::copy(data.begin(), data.end(), values);
}
