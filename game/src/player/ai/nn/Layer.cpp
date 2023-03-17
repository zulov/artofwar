#include "Layer.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <span>

#include "AFUtil.h"


Layer::Layer(std::vector<float>& w, std::vector<float>& bias) {
	prevSize = w.size() / bias.size();
	values = new float[bias.size()];
	std::fill_n(values, bias.size(), 0);

	this->bias = new float[bias.size()];
	std::ranges::copy(bias, this->bias);

	this->w = new float[w.size()];
	std::ranges::copy(w, this->w);
	valuesSpan = std::span{values, bias.size()};
}

Layer::~Layer() {
	delete []values;
	delete []bias;
	delete []w;
}

void Layer::setValues(std::span<float> data) const {
	assert(valuesSpan.size() == data.size());
	std::ranges::copy(data, valuesSpan.begin());
}

void Layer::setValues(Eigen::MatrixXf& mult) const {
	auto* bPtr = bias;
	auto* vPtr = values;
	for (int i = 0; i < mult.rows(); ++i, ++bPtr, ++vPtr) {
		*vPtr = tanh1(mult(i) + *bPtr);
	}
}
