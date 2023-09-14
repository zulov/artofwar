#include "Layer.h"
#include <span>


Layer::Layer(std::vector<float>& w, std::vector<float>& b) {
	weights = Eigen::Map<Eigen::MatrixXf>(w.data(), w.size() / b.size(), b.size()).transpose();
	bias = Eigen::Map<Eigen::VectorXf>(b.data(), b.size());
}

void Layer::setValues(std::span<float> data) {
	values = Eigen::Map<Eigen::VectorXf>(data.data(), data.size());
}

void Layer::setValues(const Eigen::VectorXf& mult) {
	values = (weights * mult + bias).array().tanh();
}
