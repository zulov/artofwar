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

Layer::~Layer() {
	delete []values;
	delete []bias;
	delete []w;
}

void Layer::setValues(double data[]) {
	//TODO musi sie zgadzac rozmiar czy to jest sprawdzane?
	std::copy(data, data + numberOfNodes, values);
}
