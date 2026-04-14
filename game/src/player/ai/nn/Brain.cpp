#include "Brain.h"

#include "Layer.h"
#include "utils/SpanUtils.h"
#include "utils/DeleteUtils.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"


Brain::Brain(const std::string& filename, const std::vector<std::string>& lines) :
	filename(filename) {
	allLayers.reserve(lines.size());

	std::vector<float> w;
	std::vector<float> b;

	for (const auto& line : lines) {
		w.clear();
		b.clear();

		parseLine(line, w, b);

		allLayers.push_back(new Layer(w, b));
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

std::string Brain::getName() const { return filename; }
