#include "Layer.h"

#include <span>


Layer::Layer(std::vector<float>& w, std::vector<float>& b) {
	weights = Eigen::Map<Eigen::MatrixXf>(w.data(), w.size() / b.size(), b.size()).transpose();
	bias = Eigen::Map<Eigen::VectorXf>(b.data(), b.size());
}

bool Layer::setInput(std::span<const float> data) {
	const bool si = sameInput(data);
	if (!si) {
		values = Eigen::Map<const Eigen::VectorXf>(data.data(), data.size());
	}

	return si;
}

void Layer::setValues(const Eigen::VectorXf& mult) {
	values = (weights * mult + bias).array().tanh();
}

bool Layer::sameInput(std::span<const float> data) {
	if (values.size() == 0) { return false; }
	for (int i = 0; i < values.size(); ++i) {
		if (data[i] != values[i]) {
			return false;
		}
	}
	return true;
}
