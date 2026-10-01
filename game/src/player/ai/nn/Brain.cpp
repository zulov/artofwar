#include "Brain.h"

#include <utility>

#include "Layer.h"
#include "utils/SpanUtils.h"
#include "utils/DeleteUtils.h"
#include "utils/FileUtils.h"


Brain::Brain(std::string filename, const std::vector<LayerData>& layers) :
	filename(std::move(filename)) {
	allLayers.reserve(layers.size());

	for (const auto& layer : layers) {
		allLayers.push_back(new Layer(layer.weights, layer.biases));
	}
}

Brain::~Brain() { clear_vector(allLayers); }

std::span<const float> Brain::decide(std::span<const float> data) {
	assert(validateSpan(__LINE__, __FILE__, data));
	bool sameInput = allLayers.front()->setInput(data);
	if (!sameInput) {
		for (size_t i = 1; i < allLayers.size(); ++i) {
			allLayers[i]->setValues(allLayers[i - 1]->getValues());
		}
	}
	const auto& result = allLayers.back()->getValues();
	const auto res1 = std::span<const float>(result.data(), result.rows());
	assert(validateSpan(__LINE__, __FILE__, res1));
	return res1;
}

const std::string& Brain::getName() const { return filename; }

int Brain::getInputSize() const { return allLayers.front()->getInputSize(); }

int Brain::getOutputSize() const { return allLayers.back()->getOutputSize(); }
