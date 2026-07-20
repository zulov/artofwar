#include "InfluenceTemplateProvider.h"

#include <cassert>
#include <memory>
#include <vector>

namespace {
	struct InfluenceTemplate {
		float coef;
		char level;
		std::vector<float> values;
	};

	std::vector<std::unique_ptr<InfluenceTemplate>>& getTemplates() {
		static std::vector<std::unique_ptr<InfluenceTemplate>> templates;
		return templates;
	}
}

const float* InfluenceTemplateProvider::get(float coef, char level) {
	assert(level > 0);

	auto& templates = getTemplates();
	for (const auto& influenceTemplate : templates) {
		if (influenceTemplate->coef == coef && influenceTemplate->level == level) {
			return influenceTemplate->values.data();
		}
	}

	const auto levelRes = level * 2 + 1;
	auto influenceTemplate = std::make_unique<InfluenceTemplate>(coef, level, std::vector<float>(levelRes * levelRes));
	auto* value = influenceTemplate->values.data();
	for (short i = -level; i <= level; ++i) {
		const auto a = i * i;
		for (short j = -level; j <= level; ++j) {
			const auto b = j * j;
			*(value++) = 1 / ((a + b) * coef + 1.f);
		}
	}
	const auto* result = influenceTemplate->values.data();
	templates.emplace_back(std::move(influenceTemplate));
	return result;
}
