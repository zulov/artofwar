#include "Brain.h"

#include <Eigen/Geometry>

#include "Game.h"
#include "Layer.h"
#include "math/SpanUtils.h"
#include "utils/DeleteUtils.h"
#include "utils/StringUtils.h"

Brain::Brain(const std::string& filename, std::vector<std::string>& lines): filename(filename) {
	std::vector<float> w;
	std::vector<float> b;
	for (auto& line : lines) {
		auto splitVec = split(line, ';');
		auto p = std::ranges::find(splitVec, "");
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

		const auto vals = prevLayer->getValues(); //TODO performance to eigen

		const auto input = Eigen::Map<Eigen::VectorXf>(vals.data(), vals.size());
		const auto weightedMatrix = Eigen::Map<Eigen::MatrixXf>(layer->getW(), layer->getPrevSize(),
		                                                        layer->getNumberOfValues()).transpose();

		Eigen::MatrixXf mult = weightedMatrix * input;

		layer->setValues(mult);
	}
	auto result = allLayers.back()->getValues();
	assert(validateSpan(__LINE__, __FILE__, result));
	return result;
}

std::string Brain::getName() const {
	return filename;
}

short Brain::getOutputSize() const {
	return allLayers.back()->getValues().size();
}