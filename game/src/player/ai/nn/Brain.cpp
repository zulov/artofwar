#include "Brain.h"

#include "Layer.h"
#include "math/SpanUtils.h"
#include "utils/DeleteUtils.h"
#include "utils/StringUtils.h"


Brain::Brain(const std::string& filename, std::vector<std::string>& lines): filename(filename) {
	std::vector<float> w;
	std::vector<float> b;
	for (auto& line : lines) {
		std::string token;
		std::istringstream tokenStream(line);
		while (std::getline(tokenStream, token, ';')) {
			if (token.empty()) {
				break;
			}

			w.push_back(toFloat(token));
		}
		std::getline(tokenStream, token, ';');

		while (std::getline(tokenStream, token, ';')) {
			b.push_back(toFloat(token));
		}

		allLayers.push_back(new Layer(w, b));
		w.clear();
		b.clear();
	}
}

Brain::~Brain() {
	clear_vector(allLayers);
}

const std::span<float> Brain::decide(std::span<const float> data) {
	assert(validateSpan(__LINE__, __FILE__, data));
	bool sameInput = allLayers.front()->setInput(data);
	if (!sameInput) {
		for (int i = 1; i < allLayers.size(); i++) {
			allLayers.at(i)
			         ->setValues(allLayers.at(i - 1)->getValues());
		}
	}

	const auto& result = allLayers.back()->getValues();
	const auto res1 = std::span(const_cast<float*>(result.data()), result.rows());
	assert(validateSpan(__LINE__, __FILE__, res1));
	return res1;
}

std::string Brain::getName() const {
	return filename;
}
