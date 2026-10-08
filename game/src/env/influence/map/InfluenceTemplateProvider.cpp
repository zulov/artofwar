#include "InfluenceTemplateProvider.h"

#include <cassert>
#include <vector>

namespace {
	struct InfluenceTemplate {
		float coef;
		char level;
		std::vector<float> values;
	};

	struct InfluenceTemplates {
		std::vector<InfluenceTemplate*> values;

		~InfluenceTemplates() {
			for (auto* value : values) delete value;
		}
	};

	InfluenceTemplates& getTemplates() {
		static InfluenceTemplates templates;
		return templates;
	}
}

const float* InfluenceTemplateProvider::get(float coef, char level) {
	assert(level > 0);

	auto& templates = getTemplates().values;
	for (const auto* influenceTemplate : templates) {
		if (influenceTemplate->coef == coef && influenceTemplate->level == level) {
			return influenceTemplate->values.data();
		}
	}

	const auto levelRes = level * 2 + 1;
	auto* influenceTemplate = new InfluenceTemplate{coef, level, std::vector<float>(levelRes * levelRes)};
	auto* value = influenceTemplate->values.data();
	for (short i = -level; i <= level; ++i) {
		const auto a = i * i;
		for (short j = -level; j <= level; ++j) {
			const auto b = j * j;
			*(value++) = 1 / ((a + b) * coef + 1.f);
		}
	}
	const auto* result = influenceTemplate->values.data();
	templates.push_back(influenceTemplate);
	return result;
}
