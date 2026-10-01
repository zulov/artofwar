#include "Layer.h"

#include <span>


Layer::Layer(const std::vector<float>& w, const std::vector<float>& b) {
	weights = Eigen::Map<const Eigen::MatrixXf>(w.data(), w.size() / b.size(), b.size()).transpose();
	bias = Eigen::Map<const Eigen::VectorXf>(b.data(), b.size());
}

bool Layer::setInput(std::span<const float> data) {
	const bool si = sameInput(data);
	if (!si) {
		values = Eigen::Map<const Eigen::VectorXf>(data.data(), data.size());
	}

	return si;
}

void Layer::setValues(const Eigen::VectorXf& mult) {
	values.noalias() = weights * mult;
	values = (values + bias).array().tanh();
}

bool Layer::sameInput(std::span<const float> data) {
	if (values.size() == 0 || data.size() != values.size()) { return false; }
	const auto mapped = Eigen::Map<const Eigen::VectorXf>(data.data(), data.size());
	return mapped == values;
}
